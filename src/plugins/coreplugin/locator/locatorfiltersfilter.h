#pragma once

#include "ilocatorfilter.h"

#include <QIcon>

namespace Core {
namespace Internal {

class Locator;

/*!
  This filter provides the user with the list of available Locator filters.
  The list is only shown when nothing has been typed yet.
 */
class LocatorFiltersFilter : public ILocatorFilter
{
    Q_OBJECT

public:
    LocatorFiltersFilter();

    // ILocatorFilter
    void prepareSearch(const QString &entry) override;
    QList<LocatorFilterEntry> matchesFor(QFutureInterface<LocatorFilterEntry> &future,
                                         const QString &entry) override;
    void accept(const LocatorFilterEntry &selection,
                QString *newText, int *selectionStart, int *selectionLength) const override;

private:
    QStringList m_filterShortcutStrings;
    QStringList m_filterDisplayNames;
    QStringList m_filterDescriptions;
    QIcon m_icon;
};

} // namespace Internal
} // namespace Core
