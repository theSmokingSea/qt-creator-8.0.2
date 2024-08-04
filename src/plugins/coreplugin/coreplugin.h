#pragma once

#include "foldernavigationwidget.h"
#include <qglobal.h>

#include <extensionsystem/iplugin.h>
#include <utils/environment.h>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace Utils {
class PathChooser;
}

namespace Core {

class FolderNavigationWidgetFactory;

namespace Internal {

class EditMode;
class MainWindow;
class Locator;

class CorePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Core.json")

public:
    CorePlugin();
    ~CorePlugin() override;

    static CorePlugin *instance();

    bool initialize(const QStringList &arguments, QString *errorMessage = nullptr) override;
    void extensionsInitialized() override;
    bool delayedInitialize() override;
    ShutdownFlag aboutToShutdown() override;
    QObject *remoteCommand(const QStringList & /* options */,
                           const QString &workingDirectory,
                           const QStringList &args) override;

    static Utils::Environment startupSystemEnvironment();
    static Utils::EnvironmentItems environmentChanges();
    static void setEnvironmentChanges(const Utils::EnvironmentItems &changes);
    static QString msgCrashpadInformation();

public slots:
    void fileOpenRequest(const QString&);


private:
    static void addToPathChooserContextMenu(Utils::PathChooser *pathChooser, QMenu *menu);
    static void setupSystemEnvironment();
    void checkSettings();
    void warnAboutCrashReporing();

    MainWindow *m_mainWindow = nullptr;
    EditMode *m_editMode = nullptr;
    Locator *m_locator = nullptr;
    FolderNavigationWidgetFactory *m_folderNavigationWidgetFactory = nullptr;
    Utils::Environment m_startupSystemEnvironment;
    Utils::EnvironmentItems m_environmentChanges;
};

} // namespace Internal
} // namespace Core
