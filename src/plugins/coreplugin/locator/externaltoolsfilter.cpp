#include "externaltoolsfilter.h"

#include <coreplugin/externaltool.h>
#include <coreplugin/externaltoolmanager.h>
#include <coreplugin/messagemanager.h>
#include <utils/qtcassert.h>

using namespace Core;
using namespace Core::Internal;

ExternalToolsFilter::ExternalToolsFilter()
{
    setId("Run external tool");
    setDisplayName(tr("Run External Tool"));
    setDescription(tr("Runs an external tool that you have set up in the options (Environment > "
                      "External Tools)."));
    setDefaultShortcutString("x");
    setPriority(Medium);
}

QList<LocatorFilterEntry> ExternalToolsFilter::matchesFor(QFutureInterface<LocatorFilterEntry> &,
                                                          const QString &)
{
    return m_results;
}

void ExternalToolsFilter::accept(const LocatorFilterEntry &selection,
                                 QString *newText, int *selectionStart, int *selectionLength) const
{
    Q_UNUSED(newText)
    Q_UNUSED(selectionStart)
    Q_UNUSED(selectionLength)
    auto tool = selection.internalData.value<ExternalTool *>();
    QTC_ASSERT(tool, return);

    auto runner = new ExternalToolRunner(tool);
    if (runner->hasError())
        MessageManager::writeFlashing(runner->errorString());
}

void ExternalToolsFilter::prepareSearch(const QString &entry)
{
    QList<LocatorFilterEntry> bestEntries;
    QList<LocatorFilterEntry> betterEntries;
    QList<LocatorFilterEntry> goodEntries;
    const Qt::CaseSensitivity entryCaseSensitivity = caseSensitivity(entry);
    const QMap<QString, ExternalTool *> externalToolsById = ExternalToolManager::toolsById();
    for (ExternalTool *tool : externalToolsById) {
        int index = tool->displayName().indexOf(entry, 0, entryCaseSensitivity);
        LocatorFilterEntry::HighlightInfo::DataType hDataType = LocatorFilterEntry::HighlightInfo::DisplayName;
        if (index < 0) {
            index = tool->description().indexOf(entry, 0, entryCaseSensitivity);
            hDataType = LocatorFilterEntry::HighlightInfo::ExtraInfo;
        }

        if (index >= 0) {
            LocatorFilterEntry filterEntry(this, tool->displayName(), QVariant::fromValue(tool));
            filterEntry.extraInfo = tool->description();
            filterEntry.highlightInfo = LocatorFilterEntry::HighlightInfo(index, entry.length(), hDataType);

            if (filterEntry.displayName.startsWith(entry, entryCaseSensitivity))
                bestEntries.append(filterEntry);
            else if (filterEntry.displayName.contains(entry, entryCaseSensitivity))
                betterEntries.append(filterEntry);
            else
                goodEntries.append(filterEntry);
        }
    }
    m_results = bestEntries + betterEntries + goodEntries;
}
