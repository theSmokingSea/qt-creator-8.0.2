#include "removefiledialog.h"
#include "ui_removefiledialog.h"

#include "filepath.h"

namespace Utils {

RemoveFileDialog::RemoveFileDialog(const FilePath &filePath, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::RemoveFileDialog)
{
    m_ui->setupUi(this);
    m_ui->fileNameLabel->setText(filePath.toUserOutput());

    // TODO
    m_ui->removeVCCheckBox->setVisible(false);
}

RemoveFileDialog::~RemoveFileDialog()
{
    delete m_ui;
}

void RemoveFileDialog::setDeleteFileVisible(bool visible)
{
    m_ui->deleteFileCheckBox->setVisible(visible);
}

bool RemoveFileDialog::isDeleteFileChecked() const
{
    return m_ui->deleteFileCheckBox->isChecked();
}

} // Utils
