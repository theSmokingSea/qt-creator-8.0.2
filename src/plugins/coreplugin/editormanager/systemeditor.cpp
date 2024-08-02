#include "systemeditor.h"

#include <utils/fileutils.h>

#include <QStringList>
#include <QUrl>
#include <QDesktopServices>

using namespace Core;
using namespace Core::Internal;
using namespace Utils;

SystemEditor::SystemEditor()
{
    setId("CorePlugin.OpenWithSystemEditor");
    setDisplayName(tr("System Editor"));
    setMimeTypes({"application/octet-stream"});
}

bool SystemEditor::startEditor(const FilePath &filePath, QString *errorMessage)
{
    Q_UNUSED(errorMessage)
    QUrl url;
    url.setPath(filePath.toString());
    url.setScheme(QLatin1String("file"));
    if (!QDesktopServices::openUrl(url)) {
        if (errorMessage)
            *errorMessage = tr("Could not open URL %1.").arg(url.toString());
        return false;
    }
    return true;
}
