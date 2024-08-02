#pragma once

#include "ilocatorfilter.h"

#include <coreplugin/editormanager/documentmodel.h>

#include <QFutureInterface>
#include <QList>
#include <QMutex>
#include <QString>

namespace Core {
namespace Internal {

class OpenDocumentsFilter : public ILocatorFilter
{
    Q_OBJECT

public:
    OpenDocumentsFilter();
    QList<LocatorFilterEntry> matchesFor(QFutureInterface<LocatorFilterEntry> &future,
                                         const QString &entry) override;
    void accept(const LocatorFilterEntry &selection,
                QString *newText, int *selectionStart, int *selectionLength) const override;
    void refresh(QFutureInterface<void> &future) override;

public slots:
    void refreshInternally();

private:
    class Entry
    {
    public:
        Utils::FilePath fileName;
        QString displayName;
    };

    QList<Entry> editors() const;

    mutable QMutex m_mutex;
    QList<Entry> m_editors;
};

} // namespace Internal
} // namespace Core
