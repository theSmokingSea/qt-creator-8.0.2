#pragma once

#include <coreplugin/locator/ilocatorfilter.h>

#include <QTimer>

#include <atomic>
#include <memory>

QT_BEGIN_NAMESPACE
class QJSEngine;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class JavaScriptFilter : public Core::ILocatorFilter
{
    Q_OBJECT
public:
    JavaScriptFilter();
    ~JavaScriptFilter() override;

    void prepareSearch(const QString &entry) override;
    QList<Core::LocatorFilterEntry> matchesFor(QFutureInterface<Core::LocatorFilterEntry> &future,
                                               const QString &entry) override;
    void accept(const Core::LocatorFilterEntry &selection, QString *newText,
                int *selectionStart, int *selectionLength) const override;

private:
    void setupEngine();

    mutable std::unique_ptr<QJSEngine> m_engine;
    QTimer m_abortTimer;
    std::atomic_bool m_aborted = false;
};

} // namespace Internal
} // namespace Core
