#pragma once

#include <coreplugin/locator/ilocatorfilter.h>

#include <QAction>
#include <QPointer>
#include <QSet>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class MenuBarFilter : public ILocatorFilter
{
    Q_OBJECT
public:
    MenuBarFilter();

    QList<LocatorFilterEntry> matchesFor(QFutureInterface<LocatorFilterEntry> &future,
                                         const QString &entry) override;
    void accept(const LocatorFilterEntry &selection, QString *newText,
                int *selectionStart, int *selectionLength) const override;
    void prepareSearch(const QString &entry) override;

private:
    QList<LocatorFilterEntry> matchesForAction(QAction *action,
                                               const QStringList &entryPath,
                                               const QStringList &path,
                                               QVector<const QMenu *> &processedMenus);
    void updateEnabledActionCache();

    QList<LocatorFilterEntry> m_entries;
    QSet<QPointer<QAction>> m_enabledActions;
};

} // namespace Internal
} // namespace Core
