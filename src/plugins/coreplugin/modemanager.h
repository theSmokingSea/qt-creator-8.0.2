#pragma once

#include <coreplugin/core_global.h>

#include <utils/id.h>

#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace Core {

class IMode;

namespace Internal {
    class MainWindow;
    class FancyTabWidget;
}

class CORE_EXPORT ModeManager : public QObject
{
    Q_OBJECT

public:
    enum class Style {
        IconsAndText,
        IconsOnly,
        Hidden
    };

    static ModeManager *instance();

    static IMode *currentMode();
    static Utils::Id currentModeId();

    static void addAction(QAction *action, int priority);
    static void addProjectSelector(QAction *action);

    static void activateMode(Utils::Id id);
    static void setFocusToCurrentMode();
    static Style modeStyle();

    static void removeMode(IMode *mode);

public slots:
    static void setModeStyle(Style layout);
    static void cycleModeStyle();

signals:
    void currentModeAboutToChange(Utils::Id mode);

    // the default argument '=0' is important for connects without the oldMode argument.
    void currentModeChanged(Utils::Id mode, Utils::Id oldMode = {});

private:
    explicit ModeManager(Internal::MainWindow *mainWindow, Internal::FancyTabWidget *modeStack);
    ~ModeManager() override;

    static void extensionsInitialized();

    static void addMode(IMode *mode);
    void currentTabAboutToChange(int index);
    void currentTabChanged(int index);

    friend class IMode;
    friend class Core::Internal::MainWindow;
};

} // namespace Core
