#pragma once

#include "utils_global.h"

#include "processreaper.h"
#include "singleton.h"

#include <QThread>

namespace Utils {
namespace Internal {
class CallerHandle;
class LauncherHandle;
class LauncherInterfacePrivate;
class ProcessLauncherImpl;
}

class QTCREATOR_UTILS_EXPORT LauncherInterface final
        : public SingletonWithOptionalDependencies<LauncherInterface, ProcessReaper>
{
public:
    static void setPathToLauncher(const QString &pathToLauncher);

private:
    friend class Internal::CallerHandle;
    friend class Internal::LauncherHandle;
    friend class Internal::ProcessLauncherImpl;

    static bool isStarted();
    static void sendData(const QByteArray &data);
    static Internal::CallerHandle *registerHandle(QObject *parent, quintptr token);
    static void unregisterHandle(quintptr token);

    LauncherInterface();
    ~LauncherInterface();

    QThread m_thread;
    Internal::LauncherInterfacePrivate *m_private;
    friend class SingletonWithOptionalDependencies<LauncherInterface, ProcessReaper>;
};

} // namespace Utils
