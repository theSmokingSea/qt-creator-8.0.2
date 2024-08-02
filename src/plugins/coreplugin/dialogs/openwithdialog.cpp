#include "openwithdialog.h"

#include <utils/fileutils.h>

#include <QPushButton>

using namespace Core;
using namespace Core::Internal;

OpenWithDialog::OpenWithDialog(const Utils::FilePath &filePath, QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    label->setText(tr("Open file \"%1\" with:").arg(filePath.fileName()));
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

    connect(buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked,
            this, &QDialog::accept);
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked,
            this, &QDialog::reject);
    connect(editorListWidget, &QListWidget::itemDoubleClicked,
        this, &QDialog::accept);
    connect(editorListWidget, &QListWidget::currentItemChanged,
            this, &OpenWithDialog::currentItemChanged);

    setOkButtonEnabled(false);
}

void OpenWithDialog::setOkButtonEnabled(bool v)
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(v);
}

void OpenWithDialog::setEditors(const QStringList &editors)
{
    for (const QString &e : editors)
        editorListWidget->addItem(e);
}

int OpenWithDialog::editor() const
{
    return editorListWidget->currentRow();
}

void OpenWithDialog::setCurrentEditor(int index)
{
    editorListWidget->setCurrentRow(index);
}

void OpenWithDialog::currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    setOkButtonEnabled(current);
}
