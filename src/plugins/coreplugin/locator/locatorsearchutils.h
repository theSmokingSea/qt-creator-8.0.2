#pragma once

#include "ilocatorfilter.h"

namespace Core {
namespace Internal {

void CORE_EXPORT runSearch(QFutureInterface<LocatorFilterEntry> &future,
                              const QList<ILocatorFilter *> &filters,
                              const QString &searchText);

} // namespace Internal
} // namespace Core
