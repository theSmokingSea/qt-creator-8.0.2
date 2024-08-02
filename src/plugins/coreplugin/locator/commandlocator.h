#pragma once

#include "ilocatorfilter.h"

namespace Core {

/* Command locators: Provides completion for a set of
 * Core::Command's by sub-string of their action's text. */
class Command;
struct CommandLocatorPrivate;

class CORE_EXPORT CommandLocator : public ILocatorFilter
{
    Q_OBJECT

public:
    CommandLocator(Utils::Id id, const QString &displayName,
                   const QString &shortCutString, QObject *parent = nullptr);
    ~CommandLocator() override;

    void appendCommand(Command *cmd);

    void prepareSearch(const QString &entry) override;
    QList<LocatorFilterEntry> matchesFor(QFutureInterface<LocatorFilterEntry> &future,
                                         const QString &entry) override;
    void accept(const LocatorFilterEntry &selection,
                QString *newText, int *selectionStart, int *selectionLength) const override;

private:
    CommandLocatorPrivate *d = nullptr;
};

} // namespace Core
