#pragma once

#include "utils_global.h"

#include "singleton.h"

#include <QThread>

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

namespace Utils {
namespace Internal { class ProcessReaperPrivate; }

class QTCREATOR_UTILS_EXPORT ProcessReaper final
        : public SingletonWithOptionalDependencies<ProcessReaper>
{
public:
    static void reap(QProcess *process, int timeoutMs = 500);

private:
    ProcessReaper();
    ~ProcessReaper();

    QThread m_thread;
    Internal::ProcessReaperPrivate *m_private;
    friend class SingletonWithOptionalDependencies<ProcessReaper>;
};

} // namespace Utils
