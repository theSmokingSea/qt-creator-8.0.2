#pragma once

#include <coreplugin/core_global.h>

#include <QCoreApplication>
#include <QMessageBox>

namespace Core {

class CORE_EXPORT RestartDialog : public QMessageBox
{
    Q_DECLARE_TR_FUNCTIONS(Core::RestartDialog)

public:
    RestartDialog(QWidget *parent, const QString &text);
};

} // namespace Core
