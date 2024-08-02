#include "futuresynchronizer.h"

/*! \class Utils::FutureSynchronizer

  \brief The FutureSynchronizer is an enhanced version of QFutureSynchronizer.
*/

namespace Utils {

FutureSynchronizer::~FutureSynchronizer()
{
    waitForFinished();
}

bool FutureSynchronizer::isEmpty() const
{
    return m_futures.isEmpty();
}

void FutureSynchronizer::waitForFinished()
{
    if (m_cancelOnWait)
        cancelAllFutures();
    for (QFuture<void> &future : m_futures)
        future.waitForFinished();
    clearFutures();
}

void FutureSynchronizer::cancelAllFutures()
{
    for (QFuture<void> &future : m_futures)
        future.cancel();
}

void FutureSynchronizer::clearFutures()
{
    m_futures.clear();
}

void FutureSynchronizer::setCancelOnWait(bool enabled)
{
    m_cancelOnWait = enabled;
}

bool FutureSynchronizer::isCancelOnWait() const
{
    return m_cancelOnWait;
}

void FutureSynchronizer::flushFinishedFutures()
{
    QList<QFuture<void>> newFutures;
    for (const QFuture<void> &future : qAsConst(m_futures)) {
        if (!future.isFinished())
            newFutures.append(future);
    }
    m_futures = newFutures;
}

} // namespace Utils
