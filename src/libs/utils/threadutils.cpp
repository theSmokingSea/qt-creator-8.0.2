#include "threadutils.h"

#include <QCoreApplication>
#include <QThread>

namespace Utils {

bool isMainThread()
{
    return QThread::currentThread() == qApp->thread();
}

} // namespace Utils
