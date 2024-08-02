#pragma once

#include "ilocatorfilter.h"

namespace Core {
namespace Internal {

class ExternalToolsFilter : public ILocatorFilter
{
    Q_OBJECT
public:
    ExternalToolsFilter();

    QList<LocatorFilterEntry> matchesFor(QFutureInterface<LocatorFilterEntry> &future,
                                         const QString &entry) override;
    void accept(const LocatorFilterEntry &selection,
                QString *newText, int *selectionStart, int *selectionLength) const override;
    void prepareSearch(const QString &entry) override;

private:
    QList<LocatorFilterEntry> m_results;
};

} // namespace Internal
} // namespace Core
