#pragma once

#include "core_global.h"

#include <QObject>

namespace Core {

class CORE_EXPORT DiffService
{
public:
    static DiffService *instance();

    DiffService();
    virtual ~DiffService();

    virtual void diffFiles(const QString &leftFileName, const QString &rightFileName) = 0;
    virtual void diffModifiedFiles(const QStringList &fileNames) = 0;
};

} // namespace Core

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Core::DiffService, "Core::DiffService")
QT_END_NAMESPACE
