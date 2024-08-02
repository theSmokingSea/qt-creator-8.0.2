#include "addtovcsdialog.h"
#include "ui_addtovcsdialog.h"

#include <QDir>
#include <QListWidgetItem>

namespace Core {
namespace Internal {

AddToVcsDialog::AddToVcsDialog(QWidget *parent, const QString &title,
                               const Utils::FilePaths &files, const QString &vcsDisplayName) :
    QDialog(parent),
    ui(new Ui::AddToVcsDialog)
{
    ui->setupUi(this);
    QString addTo = files.size() == 1
            ? tr("Add the file to version control (%1)").arg(vcsDisplayName)
            : tr("Add the files to version control (%1)").arg(vcsDisplayName);

    ui->addFilesLabel->setText(addTo);
    setWindowTitle(title);

    for (const Utils::FilePath &file : files) {
        QListWidgetItem *item = new QListWidgetItem(file.toUserOutput());
        ui->filesListWidget->addItem(item);
    }
}

AddToVcsDialog::~AddToVcsDialog()
{
    delete ui;
}

} // namespace Internal
} // namespace Core
