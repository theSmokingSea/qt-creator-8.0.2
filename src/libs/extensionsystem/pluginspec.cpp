#include "pluginspec.h"

#include "pluginspec_p.h"
#include "iplugin.h"
#include "iplugin_p.h"
#include "pluginmanager.h"

#include <utils/algorithm.h>
#include <utils/hostosinfo.h>
#include <utils/qtcassert.h>
#include <utils/stringutils.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPluginLoader>

using namespace ExtensionSystem::Internal;

namespace ExtensionSystem {
/*!
    \class ExtensionSystem::PluginDependency
    \inheaderfile extensionsystem/pluginspec.h
    \inmodule QtCreator

    \brief PluginDependency 类包含插件依赖项的名称和所需的兼容版本号。

    这反映了插件元数据中依赖对象的数据。名称和版本用于解析依赖关系。
    也就是说，会搜索具有给定名称且
    插件 \c {兼容版本 <= 依赖版本 <= 插件版本} 的插件。

    有关插件依赖和版本匹配的更多信息，请参阅 ExtensionSystem::IPlugin。
*/

/*!
    \variable ExtensionSystem::PluginDependency::name
    插件的字符串标识符。
*/

/*!
    \variable ExtensionSystem::PluginDependency::version
    插件必须匹配的版本字符串，以满足此依赖关系。
*/

/*!
    \variable ExtensionSystem::PluginDependency::type
    定义依赖是必需的还是可选的。
    \sa ExtensionSystem::PluginDependency::Type
*/

/*!
    \enum ExtensionSystem::PluginDependency::Type
    定义依赖是必需的还是可选的。
    \value Required
           依赖必须存在。
    \value Optional
           依赖不一定需要。你需要确保插件能够在没有安装此依赖的情况下加载，
           例如，你可能不会链接到依赖的库。
    \value Test
           需要强制加载依赖以运行插件的测试。
*/

/*!
    \class ExtensionSystem::PluginSpec
    \inheaderfile extensionsystem/pluginspec.h
    \inmodule QtCreator

    \brief PluginSpec 类包含插件嵌入式元数据的信息和有关插件当前状态的信息。

    随着插件经历其加载过程（参见 PluginSpec::State），插件规范还会填充更多信息。
    如果发生错误，插件规范是查找错误详细信息的地方。
*/

/*!
    \enum ExtensionSystem::PluginSpec::State
    State 枚举指示插件在加载过程中经历的状态。

    在发生错误的情况下，状态给出了出错原因的提示。

    \value  Invalid
            起点：甚至插件元数据都未读取。
    \value  Read
            插件元数据已成功读取，其信息可通过 PluginSpec 获得。
    \value  Resolved
            描述文件中给出的依赖项已成功找到，可通过 dependencySpecs() 函数获得。
    \value  Loaded
            插件的库已加载，并且插件实例已创建（可通过 plugin() 获得）。
    \value  Initialized
            插件实例的 IPlugin::initialize() 函数已被调用并返回成功值。
    \value  Running
            插件的依赖项已成功初始化，并且已调用 extensionsInitialized。加载过程完成。
    \value Stopped
            插件已关闭，即已调用插件的 IPlugin::aboutToShutdown() 函数。
    \value Deleted
            插件实例已被删除。
*/

/*!
    \class ExtensionSystem::PluginArgumentDescription
    \inheaderfile extensionsystem/pluginspec.h
    \inmodule QtCreator

    \brief PluginArgumentDescriptions 类持有插件处理的命令行参数描述列表。

    \sa PluginSpec::argumentDescriptions()
*/

/*!
    \fn uint ExtensionSystem::qHash(const ExtensionSystem::PluginDependency &value)
    \internal
*/
Utils::QHashValueType qHash(const PluginDependency &value)
{
    return qHash(value.name);
}

/*!
    \internal
*/
bool PluginDependency::operator==(const PluginDependency &other) const
{
    return name == other.name && version == other.version && type == other.type;
}

static QString typeString(PluginDependency::Type type)
{
    switch (type) {
    case PluginDependency::Optional:
        return QString(", optional");
    case PluginDependency::Test:
        return QString(", test");
    case PluginDependency::Required:
    default:
        return QString();
    }
}

/*!
    \internal
*/
QString PluginDependency::toString() const
{
    return name + " (" + version + typeString(type) + ")";
}

/*!
    \internal
*/
PluginSpec::PluginSpec()
    : d(new PluginSpecPrivate(this))
{
}

/*!
    \internal
*/
PluginSpec::~PluginSpec()
{
    delete d;
    d = nullptr;
}

/*!
    Returns the plugin name. This is valid after the PluginSpec::Read state is
    reached.
*/
QString PluginSpec::name() const
{
    return d->name;
}

/*!
    Returns the plugin version. This is valid after the PluginSpec::Read state
    is reached.
*/
QString PluginSpec::version() const
{
    return d->version;
}

/*!
    Returns the plugin compatibility version. This is valid after the
    PluginSpec::Read state is reached.
*/
QString PluginSpec::compatVersion() const
{
    return d->compatVersion;
}

/*!
    Returns the plugin vendor. This is valid after the PluginSpec::Read
    state is reached.
*/
QString PluginSpec::vendor() const
{
    return d->vendor;
}

/*!
    Returns the plugin copyright. This is valid after the PluginSpec::Read
     state is reached.
*/
QString PluginSpec::copyright() const
{
    return d->copyright;
}

/*!
    Returns the plugin license. This is valid after the PluginSpec::Read
    state is reached.
*/
QString PluginSpec::license() const
{
    return d->license;
}

/*!
    Returns the plugin description. This is valid after the PluginSpec::Read
    state is reached.
*/
QString PluginSpec::description() const
{
    return d->description;
}

/*!
    Returns the plugin URL where you can find more information about the plugin.
    This is valid after the PluginSpec::Read state is reached.
*/
QString PluginSpec::url() const
{
    return d->url;
}

/*!
    Returns the category that the plugin belongs to. Categories are used to
    group plugins together in the UI.
    Returns an empty string if the plugin does not belong to a category.
*/
QString PluginSpec::category() const
{
    return d->category;
}

QString PluginSpec::revision() const
{
    const QJsonValue revision = metaData().value("Revision");
    if (revision.isString())
        return revision.toString();
    return QString();
}

/*!
    Returns a QRegularExpression matching the platforms this plugin works on.
    An empty pattern implies all platforms.
    \since 3.0
*/

QRegularExpression PluginSpec::platformSpecification() const
{
    return d->platformSpecification;
}

/*!
    Returns whether the plugin works on the host platform.
*/
bool PluginSpec::isAvailableForHostPlatform() const
{
    return d->platformSpecification.pattern().isEmpty()
            || d->platformSpecification.match(PluginManager::platformName()).hasMatch();
}

/*!
    Returns whether the plugin is required.
*/
bool PluginSpec::isRequired() const
{
    return d->required;
}

/*!
    Returns whether the plugin has its experimental flag set.
*/
bool PluginSpec::isExperimental() const
{
    return d->experimental;
}

/*!
    Returns whether the plugin is enabled by default.
    A plugin might be disabled because the plugin is experimental, or because
    the installation settings define it as disabled by default.
*/
bool PluginSpec::isEnabledByDefault() const
{
    return d->enabledByDefault;
}

/*!
    Returns whether the plugin should be loaded at startup,
    taking into account the default enabled state, and the user's settings.

    \note This function might return \c false even if the plugin is loaded
    as a requirement of another enabled plugin.

    \sa isEffectivelyEnabled()
*/
bool PluginSpec::isEnabledBySettings() const
{
    return d->enabledBySettings;
}

/*!
    Returns whether the plugin is loaded at startup.
    \sa isEnabledBySettings()
*/
bool PluginSpec::isEffectivelyEnabled() const
{
    if (!isAvailableForHostPlatform())
        return false;
    if (isForceEnabled() || isEnabledIndirectly())
        return true;
    if (isForceDisabled())
        return false;
    return isEnabledBySettings();
}

/*!
    Returns \c true if loading was not done due to user unselecting this
    plugin or its dependencies.
*/
bool PluginSpec::isEnabledIndirectly() const
{
    return d->enabledIndirectly;
}

/*!
    Returns whether the plugin is enabled via the \c -load option on the
    command line.
*/
bool PluginSpec::isForceEnabled() const
{
    return d->forceEnabled;
}

/*!
    Returns whether the plugin is disabled via the \c -noload option on the
     command line.
*/
bool PluginSpec::isForceDisabled() const
{
    return d->forceDisabled;
}

/*!
    The plugin dependencies. This is valid after the PluginSpec::Read state is reached.
*/
QVector<PluginDependency> PluginSpec::dependencies() const
{
    return d->dependencies;
}

/*!
    Returns the plugin meta data.
*/
QJsonObject PluginSpec::metaData() const
{
    return d->metaData;
}

/*!
    Returns a list of descriptions of command line arguments the plugin processes.
*/

PluginSpec::PluginArgumentDescriptions PluginSpec::argumentDescriptions() const
{
    return d->argumentDescriptions;
}

/*!
    Returns the absolute path to the directory containing the plugin.
*/
QString PluginSpec::location() const
{
    return d->location;
}

/*!
    Returns the absolute path to the plugin.
*/
QString PluginSpec::filePath() const
{
    return d->filePath;
}

/*!
    Returns command line arguments specific to the plugin. Set at startup.
*/

QStringList PluginSpec::arguments() const
{
    return d->arguments;
}

/*!
    Sets the command line arguments specific to the plugin to \a arguments.
*/

void PluginSpec::setArguments(const QStringList &arguments)
{
    d->arguments = arguments;
}

/*!
    Adds \a argument to the command line arguments specific to the plugin.
*/

void PluginSpec::addArgument(const QString &argument)
{
    d->arguments.push_back(argument);
}


/*!
    Returns the state in which the plugin currently is.
    See the description of the PluginSpec::State enum for details.
*/
PluginSpec::State PluginSpec::state() const
{
    return d->state;
}

/*!
    Returns whether an error occurred while reading or starting the plugin.
*/
bool PluginSpec::hasError() const
{
    return d->hasError;
}

/*!
    Returns a detailed, possibly multi-line, error description in case of an
    error.
*/
QString PluginSpec::errorString() const
{
    return d->errorString;
}

/*!
    Returns whether this plugin can be used to fill in a dependency of the given
    \a pluginName and \a version.

        \sa PluginSpec::dependencies()
*/
bool PluginSpec::provides(const QString &pluginName, const QString &version) const
{
    return d->provides(pluginName, version);
}

/*!
    Returns the corresponding IPlugin instance, if the plugin library has
    already been successfully loaded. That is, the PluginSpec::Loaded state
    is reached.
*/
IPlugin *PluginSpec::plugin() const
{
    return d->plugin;
}

/*!
    Returns the list of dependencies, already resolved to existing plugin specs.
    Valid if PluginSpec::Resolved state is reached.

    \sa PluginSpec::dependencies()
*/
QHash<PluginDependency, PluginSpec *> PluginSpec::dependencySpecs() const
{
    return d->dependencySpecs;
}

/*!
    Returns whether the plugin requires any of the plugins specified by
    \a plugins.
*/
bool PluginSpec::requiresAny(const QSet<PluginSpec *> &plugins) const
{
    return Utils::anyOf(d->dependencySpecs.keys(), [this, &plugins](const PluginDependency &dep) {
        return dep.type == PluginDependency::Required
               && plugins.contains(d->dependencySpecs.value(dep));
    });
}

/*!
    Sets whether the plugin should be loaded at startup to \a value.

    \sa isEnabledBySettings()
*/
void PluginSpec::setEnabledBySettings(bool value)
{
    d->setEnabledBySettings(value);
}

PluginSpec *PluginSpec::read(const QString &filePath)
{
    auto spec = new PluginSpec;
    if (!spec->d->read(filePath)) { // not a Qt Creator plugin
        delete spec;
        return nullptr;
    }
    return spec;
}

PluginSpec *PluginSpec::read(const QStaticPlugin &plugin)
{
    auto spec = new PluginSpec;
    if (!spec->d->read(plugin)) { // not a Qt Creator plugin
        delete spec;
        return nullptr;
    }
    return spec;
}

//==========PluginSpecPrivate==================

namespace {
    const char PLUGIN_METADATA[] = "MetaData";
    const char PLUGIN_NAME[] = "Name";
    const char PLUGIN_VERSION[] = "Version";
    const char PLUGIN_COMPATVERSION[] = "CompatVersion";
    const char PLUGIN_REQUIRED[] = "Required";
    const char PLUGIN_EXPERIMENTAL[] = "Experimental";
    const char PLUGIN_DISABLED_BY_DEFAULT[] = "DisabledByDefault";
    const char VENDOR[] = "Vendor";
    const char COPYRIGHT[] = "Copyright";
    const char LICENSE[] = "License";
    const char DESCRIPTION[] = "Description";
    const char URL[] = "Url";
    const char CATEGORY[] = "Category";
    const char PLATFORM[] = "Platform";
    const char DEPENDENCIES[] = "Dependencies";
    const char DEPENDENCY_NAME[] = "Name";
    const char DEPENDENCY_VERSION[] = "Version";
    const char DEPENDENCY_TYPE[] = "Type";
    const char DEPENDENCY_TYPE_SOFT[] = "optional";
    const char DEPENDENCY_TYPE_HARD[] = "required";
    const char DEPENDENCY_TYPE_TEST[] = "test";
    const char ARGUMENTS[] = "Arguments";
    const char ARGUMENT_NAME[] = "Name";
    const char ARGUMENT_PARAMETER[] = "Parameter";
    const char ARGUMENT_DESCRIPTION[] = "Description";
}
/*!
    \internal
*/
PluginSpecPrivate::PluginSpecPrivate(PluginSpec *spec)
    : q(spec)
{}

void PluginSpecPrivate::reset()
{
    name.clear();
    version.clear();
    compatVersion.clear();
    vendor.clear();
    copyright.clear();
    license.clear();
    description.clear();
    url.clear();
    category.clear();
    location.clear();
    filePath.clear();
    state = PluginSpec::Invalid;
    hasError = false;
    errorString.clear();
    dependencies.clear();
    metaData = QJsonObject();
    loader.reset();
    staticPlugin.reset();
}

/*!
    \internal
    Returns false if the file does not represent a Qt Creator plugin.
*/
bool PluginSpecPrivate::read(const QString &fileName)
{
    qCDebug(pluginLog) << "\nReading meta data of" << fileName;
    reset();
    QFileInfo fileInfo(fileName);
    location = fileInfo.absolutePath();
    filePath = fileInfo.absoluteFilePath();
    loader.emplace();
    if (Utils::HostOsInfo::isMacHost())
        loader->setLoadHints(QLibrary::ExportExternalSymbolsHint);
    loader->setFileName(filePath);
    if (loader->fileName().isEmpty()) {
        qCDebug(pluginLog) << "Cannot open file";
        return false;
    }

    if (!readMetaData(loader->metaData()))
        return false;

    state = PluginSpec::Read;
    return true;
}

bool PluginSpecPrivate::read(const QStaticPlugin &plugin)
{
    qCDebug(pluginLog) << "\nReading meta data of static plugin";
    reset();
    staticPlugin = plugin;
    if (!readMetaData(plugin.metaData()))
        return false;

    state = PluginSpec::Read;
    return true;
}

void PluginSpecPrivate::setEnabledBySettings(bool value)
{
    enabledBySettings = value;
}

void PluginSpecPrivate::setEnabledByDefault(bool value)
{
    enabledByDefault = value;
}

void PluginSpecPrivate::setForceEnabled(bool value)
{
    forceEnabled = value;
    if (value)
        forceDisabled = false;
}

void PluginSpecPrivate::setForceDisabled(bool value)
{
    if (value)
        forceEnabled = false;
    forceDisabled = value;
}

/*!
    \internal
*/
bool PluginSpecPrivate::reportError(const QString &err)
{
    errorString = err;
    hasError = true;
    return true;
}

static inline QString msgValueMissing(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "\"%1\" is missing").arg(QLatin1String(key));
}

static inline QString msgValueIsNotAString(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a string")
            .arg(QLatin1String(key));
}

static inline QString msgValueIsNotABool(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a bool")
            .arg(QLatin1String(key));
}

static inline QString msgValueIsNotAObjectArray(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not an array of objects")
            .arg(QLatin1String(key));
}

static inline QString msgValueIsNotAMultilineString(const char *key)
{
    return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a string and not an array of strings")
            .arg(QLatin1String(key));
}

static inline QString msgInvalidFormat(const char *key, const QString &content)
{
    return QCoreApplication::translate("PluginSpec", "Value \"%2\" for key \"%1\" has invalid format")
            .arg(QLatin1String(key), content);
}

/*!
    \internal
*/
bool PluginSpecPrivate::readMetaData(const QJsonObject &pluginMetaData)
{
    qCDebug(pluginLog) << "MetaData:" << QJsonDocument(pluginMetaData).toJson();
    QJsonValue value;
    value = pluginMetaData.value(QLatin1String("IID"));
    if (!value.isString()) {
        qCDebug(pluginLog) << "Not a plugin (no string IID found)";
        return false;
    }
    if (value.toString() != PluginManager::pluginIID()) {
        qCDebug(pluginLog) << "Plugin ignored (IID does not match)";
        return false;
    }

    value = pluginMetaData.value(QLatin1String(PLUGIN_METADATA));
    if (!value.isObject()) {
        return reportError(
            ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Plugin meta data not found"));
    }
    metaData = value.toObject();

    value = metaData.value(QLatin1String(PLUGIN_NAME));
    if (value.isUndefined())
        return reportError(msgValueMissing(PLUGIN_NAME));
    if (!value.isString())
        return reportError(msgValueIsNotAString(PLUGIN_NAME));
    name = value.toString();

    value = metaData.value(QLatin1String(PLUGIN_VERSION));
    if (value.isUndefined())
        return reportError(msgValueMissing(PLUGIN_VERSION));
    if (!value.isString())
        return reportError(msgValueIsNotAString(PLUGIN_VERSION));
    version = value.toString();
    if (!isValidVersion(version))
        return reportError(msgInvalidFormat(PLUGIN_VERSION, version));

    value = metaData.value(QLatin1String(PLUGIN_COMPATVERSION));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(PLUGIN_COMPATVERSION));
    compatVersion = value.toString(version);
    if (!value.isUndefined() && !isValidVersion(compatVersion))
        return reportError(msgInvalidFormat(PLUGIN_COMPATVERSION, compatVersion));

    value = metaData.value(QLatin1String(PLUGIN_REQUIRED));
    if (!value.isUndefined() && !value.isBool())
        return reportError(msgValueIsNotABool(PLUGIN_REQUIRED));
    required = value.toBool(false);
    qCDebug(pluginLog) << "required =" << required;

    value = metaData.value(QLatin1String(PLUGIN_EXPERIMENTAL));
    if (!value.isUndefined() && !value.isBool())
        return reportError(msgValueIsNotABool(PLUGIN_EXPERIMENTAL));
    experimental = value.toBool(false);
    qCDebug(pluginLog) << "experimental =" << experimental;

    value = metaData.value(QLatin1String(PLUGIN_DISABLED_BY_DEFAULT));
    if (!value.isUndefined() && !value.isBool())
        return reportError(msgValueIsNotABool(PLUGIN_DISABLED_BY_DEFAULT));
    enabledByDefault = !value.toBool(false);
    qCDebug(pluginLog) << "enabledByDefault =" << enabledByDefault;

    if (experimental)
        enabledByDefault = false;
    enabledBySettings = enabledByDefault;

    value = metaData.value(QLatin1String(VENDOR));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(VENDOR));
    vendor = value.toString();

    value = metaData.value(QLatin1String(COPYRIGHT));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(COPYRIGHT));
    copyright = value.toString();

    value = metaData.value(QLatin1String(DESCRIPTION));
    if (!value.isUndefined() && !Utils::readMultiLineString(value, &description))
        return reportError(msgValueIsNotAString(DESCRIPTION));

    value = metaData.value(QLatin1String(URL));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(URL));
    url = value.toString();

    value = metaData.value(QLatin1String(CATEGORY));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(CATEGORY));
    category = value.toString();

    value = metaData.value(QLatin1String(LICENSE));
    if (!value.isUndefined() && !Utils::readMultiLineString(value, &license))
        return reportError(msgValueIsNotAMultilineString(LICENSE));

    value = metaData.value(QLatin1String(PLATFORM));
    if (!value.isUndefined() && !value.isString())
        return reportError(msgValueIsNotAString(PLATFORM));
    const QString platformSpec = value.toString().trimmed();
    if (!platformSpec.isEmpty()) {
        platformSpecification.setPattern(platformSpec);
        if (!platformSpecification.isValid()) {
            return reportError(::ExtensionSystem::Internal::PluginSpecPrivate::tr(
                                   "Invalid platform specification \"%1\": %2")
                                   .arg(platformSpec, platformSpecification.errorString()));
        }
    }

    value = metaData.value(QLatin1String(DEPENDENCIES));
    if (!value.isUndefined() && !value.isArray())
        return reportError(msgValueIsNotAObjectArray(DEPENDENCIES));
    if (!value.isUndefined()) {
        const QJsonArray array = value.toArray();
        for (const QJsonValue &v : array) {
            if (!v.isObject())
                return reportError(msgValueIsNotAObjectArray(DEPENDENCIES));
            QJsonObject dependencyObject = v.toObject();
            PluginDependency dep;
            value = dependencyObject.value(QLatin1String(DEPENDENCY_NAME));
            if (value.isUndefined()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Dependency: %1")
                        .arg(msgValueMissing(DEPENDENCY_NAME)));
            }
            if (!value.isString()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Dependency: %1")
                        .arg(msgValueIsNotAString(DEPENDENCY_NAME)));
            }
            dep.name = value.toString();
            value = dependencyObject.value(QLatin1String(DEPENDENCY_VERSION));
            if (!value.isUndefined() && !value.isString()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Dependency: %1")
                        .arg(msgValueIsNotAString(DEPENDENCY_VERSION)));
            }
            dep.version = value.toString();
            if (!isValidVersion(dep.version)) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Dependency: %1")
                        .arg(msgInvalidFormat(DEPENDENCY_VERSION, dep.version)));
            }
            dep.type = PluginDependency::Required;
            value = dependencyObject.value(QLatin1String(DEPENDENCY_TYPE));
            if (!value.isUndefined() && !value.isString()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Dependency: %1")
                        .arg(msgValueIsNotAString(DEPENDENCY_TYPE)));
            }
            if (!value.isUndefined()) {
                const QString typeValue = value.toString();
                if (typeValue.toLower() == QLatin1String(DEPENDENCY_TYPE_HARD)) {
                    dep.type = PluginDependency::Required;
                } else if (typeValue.toLower() == QLatin1String(DEPENDENCY_TYPE_SOFT)) {
                    dep.type = PluginDependency::Optional;
                } else if (typeValue.toLower() == QLatin1String(DEPENDENCY_TYPE_TEST)) {
                    dep.type = PluginDependency::Test;
                } else {
                    return reportError(
                        ::ExtensionSystem::Internal::PluginSpecPrivate::tr(
                            "Dependency: \"%1\" must be \"%2\" or \"%3\" (is \"%4\").")
                            .arg(QLatin1String(DEPENDENCY_TYPE),
                                 QLatin1String(DEPENDENCY_TYPE_HARD),
                                 QLatin1String(DEPENDENCY_TYPE_SOFT),
                                 typeValue));
                }
            }
            dependencies.append(dep);
        }
    }

    value = metaData.value(QLatin1String(ARGUMENTS));
    if (!value.isUndefined() && !value.isArray())
        return reportError(msgValueIsNotAObjectArray(ARGUMENTS));
    if (!value.isUndefined()) {
        const QJsonArray array = value.toArray();
        for (const QJsonValue &v : array) {
            if (!v.isObject())
                return reportError(msgValueIsNotAObjectArray(ARGUMENTS));
            QJsonObject argumentObject = v.toObject();
            PluginArgumentDescription arg;
            value = argumentObject.value(QLatin1String(ARGUMENT_NAME));
            if (value.isUndefined()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Argument: %1")
                        .arg(msgValueMissing(ARGUMENT_NAME)));
            }
            if (!value.isString()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Argument: %1")
                        .arg(msgValueIsNotAString(ARGUMENT_NAME)));
            }
            arg.name = value.toString();
            if (arg.name.isEmpty()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Argument: \"%1\" is empty")
                        .arg(QLatin1String(ARGUMENT_NAME)));
            }
            value = argumentObject.value(QLatin1String(ARGUMENT_DESCRIPTION));
            if (!value.isUndefined() && !value.isString()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Argument: %1")
                        .arg(msgValueIsNotAString(ARGUMENT_DESCRIPTION)));
            }
            arg.description = value.toString();
            value = argumentObject.value(QLatin1String(ARGUMENT_PARAMETER));
            if (!value.isUndefined() && !value.isString()) {
                return reportError(
                    ::ExtensionSystem::Internal::PluginSpecPrivate::tr("Argument: %1")
                        .arg(msgValueIsNotAString(ARGUMENT_PARAMETER)));
            }
            arg.parameter = value.toString();
            argumentDescriptions.append(arg);
            qCDebug(pluginLog) << "Argument:" << arg.name << "Parameter:" << arg.parameter
                               << "Description:" << arg.description;
        }
    }

    return true;
}

/*!
    \internal
*/
bool PluginSpecPrivate::provides(const QString &pluginName, const QString &pluginVersion) const
{
    if (QString::compare(pluginName, name, Qt::CaseInsensitive) != 0)
        return false;
    return (versionCompare(version, pluginVersion) >= 0) && (versionCompare(compatVersion, pluginVersion) <= 0);
}

/*!
    \internal
*/
const QRegularExpression &PluginSpecPrivate::versionRegExp()
{
    static const QRegularExpression reg("^([0-9]+)(?:[.]([0-9]+))?(?:[.]([0-9]+))?(?:_([0-9]+))?$");
    return reg;
}
/*!
    \internal
*/
bool PluginSpecPrivate::isValidVersion(const QString &version)
{
    return versionRegExp().match(version).hasMatch();
}

/*!
    \internal
*/
int PluginSpecPrivate::versionCompare(const QString &version1, const QString &version2)
{
    const QRegularExpressionMatch match1 = versionRegExp().match(version1);
    const QRegularExpressionMatch match2 = versionRegExp().match(version2);
    if (!match1.hasMatch())
        return 0;
    if (!match2.hasMatch())
        return 0;
    for (int i = 0; i < 4; ++i) {
        const int number1 = match1.captured(i + 1).toInt();
        const int number2 = match2.captured(i + 1).toInt();
        if (number1 < number2)
            return -1;
        if (number1 > number2)
            return 1;
    }
    return 0;
}

/*!
    \internal
*/
bool PluginSpecPrivate::resolveDependencies(const QVector<PluginSpec *> &specs)
{
    if (hasError)
        return false;
    if (state == PluginSpec::Resolved)
        state = PluginSpec::Read; // Go back, so we just re-resolve the dependencies.
    if (state != PluginSpec::Read) {
        errorString = QCoreApplication::translate("PluginSpec", "Resolving dependencies failed because state != Read");
        hasError = true;
        return false;
    }
    QHash<PluginDependency, PluginSpec *> resolvedDependencies;
    for (const PluginDependency &dependency : qAsConst(dependencies)) {
        PluginSpec * const found = Utils::findOrDefault(specs, [&dependency](PluginSpec *spec) {
            return spec->provides(dependency.name, dependency.version);
        });
        if (!found) {
            if (dependency.type == PluginDependency::Required) {
                hasError = true;
                if (!errorString.isEmpty())
                    errorString.append(QLatin1Char('\n'));
                errorString.append(QCoreApplication::translate("PluginSpec", "Could not resolve dependency '%1(%2)'")
                    .arg(dependency.name).arg(dependency.version));
            }
            continue;
        }
        resolvedDependencies.insert(dependency, found);
    }
    if (hasError)
        return false;

    dependencySpecs = resolvedDependencies;

    state = PluginSpec::Resolved;

    return true;
}

// returns the plugins that it actually indirectly enabled
QVector<PluginSpec *> PluginSpecPrivate::enableDependenciesIndirectly(bool enableTestDependencies)
{
    if (!q->isEffectivelyEnabled()) // plugin not enabled, nothing to do
        return {};
    QVector<PluginSpec *> enabled;
    for (auto it = dependencySpecs.cbegin(), end = dependencySpecs.cend(); it != end; ++it) {
        if (it.key().type != PluginDependency::Required
                && (!enableTestDependencies || it.key().type != PluginDependency::Test))
            continue;
        PluginSpec *dependencySpec = it.value();
        if (!dependencySpec->isEffectivelyEnabled()) {
            dependencySpec->d->enabledIndirectly = true;
            enabled << dependencySpec;
        }
    }
    return enabled;
}

/*!
    \internal
*/
bool PluginSpecPrivate::loadLibrary()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Resolved) {
        if (state == PluginSpec::Loaded)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Loading the library failed because state != Resolved");
        hasError = true;
        return false;
    }
    if (loader && !loader->load()) {
        hasError = true;
        errorString = QDir::toNativeSeparators(filePath) + QString::fromLatin1(": ")
                      + loader->errorString();
        return false;
    }
    auto *pluginObject = loader ? qobject_cast<IPlugin *>(loader->instance())
                                : qobject_cast<IPlugin *>(staticPlugin->instance());
    if (!pluginObject) {
        hasError = true;
        errorString = QCoreApplication::translate("PluginSpec", "Plugin is not valid (does not derive from IPlugin)");
        if (loader)
            loader->unload();
        return false;
    }
    state = PluginSpec::Loaded;
    plugin = pluginObject;
    plugin->d->pluginSpec = q;
    return true;
}

/*!
    \internal
*/
bool PluginSpecPrivate::initializePlugin()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Loaded) {
        if (state == PluginSpec::Initialized)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Initializing the plugin failed because state != Loaded");
        hasError = true;
        return false;
    }
    if (!plugin) {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to initialize");
        hasError = true;
        return false;
    }
    QString err;
    if (!plugin->initialize(arguments, &err)) {
        errorString = QCoreApplication::translate("PluginSpec", "Plugin initialization failed: %1").arg(err);
        hasError = true;
        return false;
    }
    state = PluginSpec::Initialized;
    return true;
}

/*!
    \internal
*/
bool PluginSpecPrivate::initializeExtensions()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Initialized) {
        if (state == PluginSpec::Running)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Cannot perform extensionsInitialized because state != Initialized");
        hasError = true;
        return false;
    }
    if (!plugin) {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to perform extensionsInitialized");
        hasError = true;
        return false;
    }
    plugin->extensionsInitialized();
    state = PluginSpec::Running;
    return true;
}

/*!
    \internal
*/
bool PluginSpecPrivate::delayedInitialize()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Running)
        return false;
    if (!plugin) {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to perform delayedInitialize");
        hasError = true;
        return false;
    }
    return plugin->delayedInitialize();
}

/*!
    \internal
*/
IPlugin::ShutdownFlag PluginSpecPrivate::stop()
{
    if (!plugin)
        return IPlugin::SynchronousShutdown;
    state = PluginSpec::Stopped;
    return plugin->aboutToShutdown();
}

/*!
    \internal
*/
void PluginSpecPrivate::kill()
{
    if (!plugin)
        return;
    delete plugin;
    plugin = nullptr;
    state = PluginSpec::Deleted;
}

} // ExtensionSystem
