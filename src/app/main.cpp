#include <app/app_version.h>
#include <extensionsystem/iplugin.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <qtsingleapplication.h>

#include <utils/algorithm.h>
#include <utils/environment.h>
#include <utils/hostosinfo.h>
#include <utils/optional.h>
#include <utils/qtcsettings.h>
#include <utils/singleton.h>
#include <utils/temporarydirectory.h>
#include <utils/terminalcommand.h>

#include <QDir>
#include <QFontDatabase>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QScopeGuard>
#include <QStyle>
#include <QTextStream>
#include <QThreadPool>
#include <QTimer>
#include <QTranslator>
#include <QUrl>
#include <QVariant>

#include <QSysInfo>

#include <QNetworkProxyFactory>

#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QTextCodec>

#include <vector>

using namespace ExtensionSystem;

enum { OptionIndent = 4, DescriptionIndent = 34 };

const char corePluginNameC[] = "Core";
const char fixedOptionsC[] =
" [OPTION]... [FILE]...\n"
"Options:\n"
"    -help                         Display this help\n"
"    -version                      Display program version\n"
"    -client                       Attempt to connect to already running first instance\n"
"    -settingspath <path>          Override the default path where user settings are stored\n"
"    -installsettingspath <path>   Override the default path from where user-independent settings are read\n"
"    -temporarycleansettings, -tcs Use clean settings for debug or testing reasons\n"
"    -pid <pid>                    Attempt to connect to instance given by pid\n"
"    -block                        Block until editor is closed\n"
"    -pluginpath <path>            Add a custom search path for plugins\n";

const char HELP_OPTION1[] = "-h";
const char HELP_OPTION2[] = "-help";
const char HELP_OPTION3[] = "/h";
const char HELP_OPTION4[] = "--help";
const char VERSION_OPTION[] = "-version";
const char CLIENT_OPTION[] = "-client";
const char SETTINGS_OPTION[] = "-settingspath";
const char INSTALL_SETTINGS_OPTION[] = "-installsettingspath";
const char TEST_OPTION[] = "-test";
const char TEMPORARY_CLEAN_SETTINGS1[] = "-temporarycleansettings";
const char TEMPORARY_CLEAN_SETTINGS2[] = "-tcs";
const char PID_OPTION[] = "-pid";
const char BLOCK_OPTION[] = "-block";
const char PLUGINPATH_OPTION[] = "-pluginpath";
const char USER_LIBRARY_PATH_OPTION[] = "-user-library-path"; // hidden option for qtcreator.sh

using PluginSpecSet = QVector<PluginSpec *>;

// Helpers for displaying messages. Note that there is no console on Windows.

// Format as <pre> HTML
static inline QString toHtml(const QString &t)
{
    QString res = t;
    res.replace(QLatin1Char('&'), QLatin1String("&amp;"));
    res.replace(QLatin1Char('<'), QLatin1String("&lt;"));
    res.replace(QLatin1Char('>'), QLatin1String("&gt;"));
    res.insert(0, QLatin1String("<html><pre>"));
    res.append(QLatin1String("</pre></html>"));
    return res;
}

static void displayHelpText(const QString &t)
{
    if (Utils::HostOsInfo::isWindowsHost() && qApp)
        QMessageBox::information(nullptr, QLatin1String(Core::Constants::IDE_DISPLAY_NAME), toHtml(t));
    else
        qWarning("%s", qPrintable(t));
}

static void displayError(const QString &t)
{
    if (Utils::HostOsInfo::isWindowsHost() && qApp)
        QMessageBox::critical(nullptr, QLatin1String(Core::Constants::IDE_DISPLAY_NAME), t);
    else
        qCritical("%s", qPrintable(t));
}

static void printVersion(const PluginSpec *coreplugin)
{
    QString version;
    QTextStream str(&version);
    str << '\n' << Core::Constants::IDE_DISPLAY_NAME << ' ' << coreplugin->version()<< " based on Qt " << qVersion() << "\n\n";
    PluginManager::formatPluginVersions(str);
    str << '\n' << coreplugin->copyright() << '\n';
    displayHelpText(version);
}

static void printHelp(const QString &a0)
{
    QString help;
    QTextStream str(&help);
    str << "Usage: " << a0 << fixedOptionsC;
    PluginManager::formatOptions(str, OptionIndent, DescriptionIndent);
    PluginManager::formatPluginOptions(str, OptionIndent, DescriptionIndent);
    displayHelpText(help);
}

QString applicationDirPath(char *arg = nullptr)
{
    static QString dir;

    if (arg)
        dir = QFileInfo(QString::fromLocal8Bit(arg)).dir().absolutePath();

    if (QCoreApplication::instance())
        return QApplication::applicationDirPath();

    return dir;
}

static QString resourcePath()
{
    return QDir::cleanPath(applicationDirPath() + '/' + RELATIVE_DATA_PATH);
}

static inline QString msgCoreLoadFailure(const QString &why)
{
    return QCoreApplication::translate("Application", "Failed to load core: %1").arg(why);
}

static inline int askMsgSendFailed()
{
    return QMessageBox::question(nullptr, QApplication::translate("Application","Could not send message"),
                QCoreApplication::translate("Application", "Unable to send command line arguments "
                                            "to the already running instance. It does not appear to "
                                            "be responding. Do you want to start a new instance of "
                                            "%1?").arg(Core::Constants::IDE_DISPLAY_NAME),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Retry,
                QMessageBox::Retry);
}

// 获取插件路径
static inline QStringList getPluginPaths()
{
    QStringList rc(QDir::cleanPath(QApplication::applicationDirPath()
                                   + '/' + RELATIVE_PLUGIN_PATH));
    // Local plugin path: <localappdata>/plugins/<ideversion>
    //    where <localappdata> is e.g.
    //    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
    //    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
    //    "~/Library/Application Support/QtProject/Qt Creator" on Mac
    QString pluginPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    if (Utils::HostOsInfo::isAnyUnixHost() && !Utils::HostOsInfo::isMacHost()) {
        pluginPath += QLatin1String("/data");
    }
    pluginPath += QLatin1Char('/')
            + QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR)
            + QLatin1Char('/');
    pluginPath += QLatin1String(Utils::HostOsInfo::isMacHost() ?
                                    Core::Constants::IDE_DISPLAY_NAME :
                                    Core::Constants::IDE_ID);
    pluginPath += QLatin1String("/plugins/");
    // Qt Creator X.Y.Z can load plugins from X.Y.(Z-1) etc, so add current and previous
    // patch versions
    const QString minorVersion = QString::number(IDE_VERSION_MAJOR) + '.'
                                 + QString::number(IDE_VERSION_MINOR) + '.';
    const int minPatchVersion
        = qMin(IDE_VERSION_RELEASE,
               QVersionNumber::fromString(Core::Constants::IDE_VERSION_COMPAT).microVersion());
    for (int patchVersion = IDE_VERSION_RELEASE; patchVersion >= minPatchVersion; --patchVersion)
        rc.push_back(pluginPath + minorVersion + QString::number(patchVersion));
    return rc;
}

// 检查默认安装设置是否包含实际安装设置的配置项
static void setupInstallSettings(QString &installSettingspath)
{
    if (!installSettingspath.isEmpty() && !QFileInfo(installSettingspath).isDir()) {
        displayError(QString("-installsettingspath \"%0\" needs to be the path where a %1/%2.ini exist.").arg(installSettingspath,
            QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR), QLatin1String(Core::Constants::IDE_CASED_ID)));
        installSettingspath.clear();
    }
    static const char kInstallSettingsKey[] = "Settings/InstallSettings";
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope,
        installSettingspath.isEmpty() ? resourcePath() : installSettingspath);

    // 检查默认安装设置是否包含实际安装设置的配置项。
    // 这可以是绝对路径，也可以是相对于applicationDirPath()的路径。
    // 结果的解释方式类似于-settingspath，但是应用于SystemScope。
    // 由于即将到来的sdktool拆分，新的安装设置可能会再次重定向。所以尝试这个过程几次。
    // （只有第一次使用QSettings::UserScope，以允许在用户设置中设置安装设置路径。）
    QSettings::Scope scope = QSettings::UserScope;
    int count = 0;
    bool containsInstallSettingsKey = false;
    do {
        QSettings installSettings(QSettings::IniFormat, scope,
                                  QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR),
                                  QLatin1String(Core::Constants::IDE_CASED_ID));
        containsInstallSettingsKey = installSettings.contains(kInstallSettingsKey);
        if (containsInstallSettingsKey) {
            QString newInstallSettingsPath = installSettings.value(kInstallSettingsKey).toString();
            if (QDir::isRelativePath(newInstallSettingsPath))
                newInstallSettingsPath = applicationDirPath() + '/' + newInstallSettingsPath;
            QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, newInstallSettingsPath);
        }
        scope = QSettings::SystemScope; // UserScope only the first time we check
        ++count;
    } while (containsInstallSettingsKey && count < 3);
}

static Utils::QtcSettings *createUserSettings()
{
    return new Utils::QtcSettings(QSettings::IniFormat,
                                  QSettings::UserScope,
                                  QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR),
                                  QLatin1String(Core::Constants::IDE_CASED_ID));
}

// 根据用户设置及系统环境变量的是否设置qt的高分辨率判断是否启用 Qt::AA_EnableHighDpiScaling)
static void setHighDpiEnvironmentVariable()
{
    std::unique_ptr<QSettings> settings(createUserSettings());

    const bool defaultValue = Utils::HostOsInfo::isWindowsHost();
    const bool enableHighDpiScaling = settings->value("Core/EnableHighDpiScaling", defaultValue).toBool();

    static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
    if (enableHighDpiScaling
            && !qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO) // legacy in 5.6, but still functional
            && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
            && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
            && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }
}

// 加载字体
void loadFonts()
{
    const QDir dir(resourcePath() + "/fonts/");

    const QFileInfoList fonts = dir.entryInfoList(QStringList("*.ttf"), QDir::Files);
    for (const QFileInfo &fileInfo : fonts)
        QFontDatabase::addApplicationFont(fileInfo.absoluteFilePath());
}

// 命令行参数相关
struct Options
{
    QString settingsPath;
    QString installSettingsPath;
    QStringList customPluginPaths;
    // list of arguments that were handled and not passed to the application or plugin manager
    QStringList preAppArguments;
    // list of arguments to be passed to the application or plugin manager
    std::vector<char *> appArguments;
    Utils::optional<QString> userLibraryPath;
    bool hasTestOption = false;
    bool wantsCleanSettings = false;
};

// 解析命令行参数
Options parseCommandLine(int argc, char *argv[])
{
    Options options;
    auto it = argv;
    const auto end = argv + argc;
    while (it != end) {
        const auto arg = QString::fromLocal8Bit(*it);
        const bool hasNext = it + 1 != end;
        const auto nextArg = hasNext ? QString::fromLocal8Bit(*(it + 1)) : QString();

        if (arg == SETTINGS_OPTION && hasNext) {
            ++it;
            options.settingsPath = QDir::fromNativeSeparators(nextArg);
            options.preAppArguments << arg << nextArg;
        } else if (arg == INSTALL_SETTINGS_OPTION && hasNext) {
            ++it;
            options.installSettingsPath = QDir::fromNativeSeparators(nextArg);
            options.preAppArguments << arg << nextArg;
        } else if (arg == PLUGINPATH_OPTION && hasNext) {
            ++it;
            options.customPluginPaths += QDir::fromNativeSeparators(nextArg);
            options.preAppArguments << arg << nextArg;
        } else if (arg == USER_LIBRARY_PATH_OPTION && hasNext) {
            ++it;
            options.userLibraryPath = nextArg;
            options.preAppArguments << arg << nextArg;
        } else if (arg == TEMPORARY_CLEAN_SETTINGS1 || arg == TEMPORARY_CLEAN_SETTINGS2) {
            options.wantsCleanSettings = true;
            options.preAppArguments << arg;
        } else { // arguments that are still passed on to the application
            if (arg == TEST_OPTION)
                options.hasTestOption = true;
            options.appArguments.push_back(*it);
        }
        ++it;
    }
    return options;
}

// 处理命令行参数 及重启相关
class Restarter
{
public:
    Restarter(int argc, char *argv[])
    {
        Q_UNUSED(argc)
        m_executable = QString::fromLocal8Bit(argv[0]);
        m_workingPath = QDir::currentPath();
    }

    void setArguments(const QStringList &args) { m_args = args; }

    QString executable() const { return m_executable; }
    QStringList arguments() const { return m_args; }
    QString workingPath() const { return m_workingPath; }

    int restartOrExit(int exitCode)
    {
        return qApp->property("restart").toBool() ? restart(exitCode) : exitCode;
    }

    int restart(int exitCode)
    {
        QProcess::startDetached(m_executable, m_args, m_workingPath);
        return exitCode;
    }

private:
    QString m_executable;
    QStringList m_args;
    QString m_workingPath;
};

QStringList lastSessionArgument()
{
    // 在这里使用内部信息并不是特别优雅，无论如何
    const bool hasProjectExplorer = Utils::anyOf(PluginManager::plugins(),
                                                 Utils::equal(&PluginSpec::name,
                                                              QString("ProjectExplorer")));
    return hasProjectExplorer ? QStringList({"-lastsession"}) : QStringList();
}



int main(int argc, char **argv)
{
    // 进行应用程序环境变量设置

    Restarter restarter(argc, argv);

    Utils::Environment::systemEnvironment();// 在我们做任何改变之前缓存系统环境

    // 手动确定各种命令行选项，我们不能使用插件管理器的常规方式，因为设置可能会改变插件管理器的行为方式
    Options options = parseCommandLine(argc, argv);
    applicationDirPath(argv[0]);

    // 如果设置QTC_DO_NOT_PROPAGATE_LD_PRELOAD，则取消系统环境变量中的LD_PRELOAD设置
    if (qEnvironmentVariableIsSet("QTC_DO_NOT_PROPAGATE_LD_PRELOAD")) {
        Utils::Environment::modifySystemEnvironment(
            {{"LD_PRELOAD", "", Utils::EnvironmentItem::Unset}});
    }

    // 命令行中是否存在用户库路径
    if (options.userLibraryPath) {
        if ((*options.userLibraryPath).isEmpty()) {
            Utils::Environment::modifySystemEnvironment(
                {{"LD_LIBRARY_PATH", "", Utils::EnvironmentItem::Unset}});
        } else {
            Utils::Environment::modifySystemEnvironment(
                {{"LD_LIBRARY_PATH", *options.userLibraryPath, Utils::EnvironmentItem::SetEnabled}});
        }
    }

    // 兼容嵌入式设备
    if (!qEnvironmentVariableIsSet("QT_OPENGL"))
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

    // 兼容早期版本的Ubuntu
    if (qEnvironmentVariableIsSet("QTCREATOR_DISABLE_NATIVE_MENUBAR")
            || qgetenv("XDG_CURRENT_DESKTOP").startsWith("Unity")) {
        QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    }

    // mac环境下，如果有Rosetta，则禁用了正则表达式的即时编译
    if (Utils::HostOsInfo::isRunningUnderRosetta()) {
        // work around QTBUG-97085: QRegularExpression jitting is not reentrant under Rosetta
        qputenv("QT_ENABLE_REGEXP_JIT", "0");
    }

    // 创建专用的应用程序临时目录 XXXXXX会被替换成唯一标识，每次启动都不一样
    qDebug() << "temp directory: " << QDir::tempPath() + "/" + Core::Constants::IDE_CASED_ID + "-XXXXXX";
    Utils::TemporaryDirectory::setMasterTemporaryDirectory(QDir::tempPath() + "/" + Core::Constants::IDE_CASED_ID + "-XXXXXX");

    QScopedPointer<Utils::TemporaryDirectory> temporaryCleanSettingsDir;
    if (options.settingsPath.isEmpty() && (options.hasTestOption || options.wantsCleanSettings)) {
        temporaryCleanSettingsDir.reset(new Utils::TemporaryDirectory("qtc-test-settings"));
        if (!temporaryCleanSettingsDir->isValid())
            return 1;
        options.settingsPath = temporaryCleanSettingsDir->path().path();
    }
    if (!options.settingsPath.isEmpty())
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, options.settingsPath);

    // 是否加载配置项
    // 必须在创建任何 QSettings 类之前完成 强制所有平台都使用ini文件保存配置
    QSettings::setDefaultFormat(QSettings::IniFormat);
    setupInstallSettings(options.installSettingsPath);
    // 插件管理器接管了这个设置对象的控制权

    // 设置分辨率
    setHighDpiEnvironmentVariable();

    SharedTools::QtSingleApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    int numberofArguments = static_cast<int>(options.appArguments.size());

    SharedTools::QtSingleApplication app((QLatin1String(Core::Constants::IDE_DISPLAY_NAME)),
                                         numberofArguments,
                                         options.appArguments.data());
    QCoreApplication::setApplicationName(Core::Constants::IDE_CASED_ID);
    QCoreApplication::setApplicationVersion(QLatin1String(Core::Constants::IDE_VERSION_LONG));
    QCoreApplication::setOrganizationName(QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR));
    QGuiApplication::setApplicationDisplayName(Core::Constants::IDE_DISPLAY_NAME);

    auto cleanup = qScopeGuard([] { Utils::Singleton::deleteAll(); });

    const QStringList pluginArguments = app.arguments();

    // 初始化全局设置并使用 QApplication::applicationDirPath 重新设置安装设置
    setupInstallSettings(options.installSettingsPath);
    // 获取用户设置
    Utils::QtcSettings *settings = createUserSettings();
    // 获取全局设置
    Utils::QtcSettings *globalSettings
        = new Utils::QtcSettings(QSettings::IniFormat,
                                 QSettings::SystemScope,
                                 QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR),
                                 QLatin1String(Core::Constants::IDE_CASED_ID));
    Utils::TerminalCommand::setSettings(settings);

    // 加载应用程序字体
    loadFonts();

    if (Utils::HostOsInfo::isWindowsHost()
            && !qFuzzyCompare(qApp->devicePixelRatio(), 1.0)
            && QApplication::style()->objectName().startsWith(
                QLatin1String("windows"), Qt::CaseInsensitive)) {
        QApplication::setStyle(QLatin1String("fusion"));
    }

    // 设置线程池最大线程数，至少有4个线程(为了并行处理，如编译、索引、代码分析等）
    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(qMax(4, 2 * threadCount));

    const QString libexecPath = QCoreApplication::applicationDirPath()
            + '/' + RELATIVE_LIBEXEC_PATH;

    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setAttribute(Qt::AA_DisableWindowContextHelpButton);

    // 创建插件管理器并初始化配置信息
    PluginManager pluginManager;
    PluginManager::setPluginIID(QLatin1String("org.qt-project.Qt.QtCreatorPlugin"));
    PluginManager::setGlobalSettings(globalSettings);
    PluginManager::setSettings(settings);

    // 设置语言国际化
    QTranslator translator;
    QTranslator qtTranslator;
    QStringList uiLanguages = QLocale::system().uiLanguages();
    QString overrideLanguage = settings->value(QLatin1String("General/OverrideLanguage")).toString();
    if (!overrideLanguage.isEmpty())
        uiLanguages.prepend(overrideLanguage);
    const QString &creatorTrPath = resourcePath() + "/translations";
    for (QString locale : qAsConst(uiLanguages)) {
        locale = QLocale(locale).name();
        if (translator.load("qtcreator_" + locale, creatorTrPath)) {
            const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            // Binary installer puts Qt tr files into creatorTrPath
            if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, creatorTrPath)) {
                app.installTranslator(&translator);
                app.installTranslator(&qtTranslator);
                app.setProperty("qtc_locale", locale);
                break;
            }
            Q_UNUSED(translator.load(QString())); // unload()
        } else if (locale == QLatin1String("C") /* overrideLanguage == "English" */) {
            // use built-in
            break;
        } else if (locale.startsWith(QLatin1String("en")) /* "English" is built-in */) {
            // use built-in
            break;
        }
    }

    QByteArray overrideCodecForLocale = settings->value("General/OverrideCodecForLocale").toByteArray();
    if (!overrideCodecForLocale.isEmpty())
        QTextCodec::setCodecForLocale(QTextCodec::codecForName(overrideCodecForLocale));

    app.setDesktopFileName("org.qt-project.qtcreator.desktop");

    //  确保我们遵守系统的代理设置
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    // Load
    // 设置插件搜索路径并解析各个插件
    const QStringList pluginPaths = getPluginPaths() + options.customPluginPaths;
    PluginManager::setPluginPaths(pluginPaths);
    QMap<QString, QString> foundAppOptions;
    // 设置插件管理器参数与插件参数
    qDebug() << "plugin argument size " << pluginArguments.size();

    if (pluginArguments.size() > 1) {
        QMap<QString, bool> appOptions;
        appOptions.insert(QLatin1String(HELP_OPTION1), false);
        appOptions.insert(QLatin1String(HELP_OPTION2), false);
        appOptions.insert(QLatin1String(HELP_OPTION3), false);
        appOptions.insert(QLatin1String(HELP_OPTION4), false);
        appOptions.insert(QLatin1String(VERSION_OPTION), false);
        appOptions.insert(QLatin1String(CLIENT_OPTION), false);
        appOptions.insert(QLatin1String(PID_OPTION), true);
        appOptions.insert(QLatin1String(BLOCK_OPTION), false);
        QString errorMessage;
        if (!PluginManager::parseOptions(pluginArguments, appOptions, &foundAppOptions, &errorMessage)) {
            displayError(errorMessage);
            printHelp(QFileInfo(app.applicationFilePath()).baseName());
            return -1;
        }
    }
    restarter.setArguments(options.preAppArguments + PluginManager::argumentsForRestart()
                           + lastSessionArgument());
    // if settingspath is not provided we need to pass on the settings in use
    const QString settingspath = options.preAppArguments.contains(QLatin1String(SETTINGS_OPTION))
            ? QString() : options.settingsPath;
    const PluginManager::ProcessData processData = { restarter.executable(),
            options.preAppArguments + PluginManager::argumentsForRestart(), restarter.workingPath(),
            settingspath};
    PluginManager::setCreatorProcessData(processData);

    // 查找核心插件
    const PluginSpecSet plugins = PluginManager::plugins();
    PluginSpec *coreplugin = nullptr;
    for (PluginSpec *spec : plugins) {
        if (spec->name() == QLatin1String(corePluginNameC)) {  // corePluginNameC = "Core"
            coreplugin = spec;
            qDebug() << "coreplugin " << spec->name();
            break;
        }
    }
    if (!coreplugin) {  // 如果核心插件不存在
        QString nativePaths = QDir::toNativeSeparators(pluginPaths.join(QLatin1Char(',')));
        const QString reason = QCoreApplication::translate("Application", "Could not find Core plugin in %1").arg(nativePaths);
        displayError(msgCoreLoadFailure(reason));
        return 1;
    }
    if (!coreplugin->isEffectivelyEnabled()) {  // 如果核心插件被禁用
        const QString reason = QCoreApplication::translate("Application", "Core plugin is disabled.");
        displayError(msgCoreLoadFailure(reason));
        return 1;
    }
    if (coreplugin->hasError()) {  // 如果核心插件加载失败
        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }
    // 如果命令行参数 包含 --version， 则打印版本便退出
    if (foundAppOptions.contains(QLatin1String(VERSION_OPTION))) {
        printVersion(coreplugin);
        return 0;
    }
    // 如果命令行选项包含help，则打印帮助并退出
    if (foundAppOptions.contains(QLatin1String(HELP_OPTION1))
            || foundAppOptions.contains(QLatin1String(HELP_OPTION2))
            || foundAppOptions.contains(QLatin1String(HELP_OPTION3))
            || foundAppOptions.contains(QLatin1String(HELP_OPTION4))) {
        printHelp(QFileInfo(app.applicationFilePath()).baseName());
        return 0;
    }

    // 设置进程id 不知道用途
    qint64 pid = -1;
    if (foundAppOptions.contains(QLatin1String(PID_OPTION))) {
        QString pidString = foundAppOptions.value(QLatin1String(PID_OPTION));
        bool pidOk;
        qint64 tmpPid = pidString.toInt(&pidOk);
        if (pidOk)
            pid = tmpPid;
    }

    bool isBlock = foundAppOptions.contains(QLatin1String(BLOCK_OPTION));
    if (app.isRunning() && (pid != -1 || isBlock
                            || foundAppOptions.contains(QLatin1String(CLIENT_OPTION)))) {
        app.setBlock(isBlock);
        if (app.sendMessage(PluginManager::serializedArguments(), 5000 /*timeout*/, pid))
            return 0;

        // Message could not be send, maybe it was in the process of quitting
        if (app.isRunning(pid)) {
            // Nah app is still running, ask the user
            int button = askMsgSendFailed();
            while (button == QMessageBox::Retry) {
                if (app.sendMessage(PluginManager::serializedArguments(), 5000 /*timeout*/, pid))
                    return 0;
                if (!app.isRunning(pid)) // App quit while we were trying so start a new creator
                    button = QMessageBox::Yes;
                else
                    button = askMsgSendFailed();
            }
            if (button == QMessageBox::No)
                return -1;
        }
    }

    qDebug() << "pid: " << pid;

    // 检查插件是否存在问题，如果存在问题会在弹窗打印出来，提示用户
    PluginManager::checkForProblematicPlugins();

    // 加载插件：
    //      根据 PluginsLoading、PluginsInitializing、PluginsDelayedInitializing顺序加载，最后UpAndRunning
    //      PluginsLoading  正在加载插件
    //      PluginsInitializing  正在初始化插件
    //      PluginsLoading  延迟初始化插件
    //      UpAndRunning  初始化完成，QtCreator完全启动并运行
    PluginManager::loadPlugins();

    // 如果核心插件加载失败
    if (coreplugin->hasError()) {
        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }

    // Set up remote arguments.
    QObject::connect(&app, &SharedTools::QtSingleApplication::messageReceived,
                     &pluginManager, &PluginManager::remoteArguments);

    QObject::connect(&app, SIGNAL(fileOpenRequest(QString)), coreplugin->plugin(),
                     SLOT(fileOpenRequest(QString)));

    // shutdown plugin manager on the exit
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &pluginManager, &PluginManager::shutdown);


    return restarter.restartOrExit(app.exec());
}
