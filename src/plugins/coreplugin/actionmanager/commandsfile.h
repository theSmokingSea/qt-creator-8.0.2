#pragma once

#include <utils/filepath.h>

#include <QObject>
#include <QString>
#include <QMap>

QT_FORWARD_DECLARE_CLASS(QKeySequence)

namespace Core {
namespace Internal {

struct ShortcutItem;

class CommandsFile : public QObject
{
    Q_OBJECT

public:
    CommandsFile(const Utils::FilePath &filePath);

    QMap<QString, QList<QKeySequence> > importCommands() const;
    bool exportCommands(const QList<ShortcutItem *> &items);

private:
    Utils::FilePath m_filePath;
};

} // namespace Internal
} // namespace Core
