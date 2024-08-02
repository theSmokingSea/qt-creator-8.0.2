#pragma once

#include "utils_global.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QNetworkProxy;
QT_END_NAMESPACE

namespace Utils {

namespace Ui {
class ProxyCredentialsDialog;
}

class QTCREATOR_UTILS_EXPORT ProxyCredentialsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxyCredentialsDialog(const QNetworkProxy &proxy, QWidget *parent = nullptr);
    ~ProxyCredentialsDialog() override;

    QString userName() const;
    void setUserName(const QString &username);
    QString password() const;
    void setPassword(const QString &passwd);

private:
    Ui::ProxyCredentialsDialog *ui;
};

} // namespace Utils
