#pragma once

#include <utils/filepath.h>

#include <QDialog>

namespace Core {
namespace Internal {

namespace Ui { class AddToVcsDialog; }

class AddToVcsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddToVcsDialog(QWidget *parent, const QString &title,
                            const Utils::FilePaths &files, const QString &vcsDisplayName);
    ~AddToVcsDialog() override;

private:
    Ui::AddToVcsDialog *ui;
};


} // namespace Internal
} // namespace Core
