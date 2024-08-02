#pragma once

#include "utils_global.h"

#include <QList>
#include <QString>

namespace Utils {

class QTCREATOR_UTILS_EXPORT ProcessInfo
{
public:
    qint64 processId = 0;
    QString executable;
    QString commandLine;

    bool operator<(const ProcessInfo &other) const;

    static QList<ProcessInfo> processInfoList();
};

} // namespace Utils
