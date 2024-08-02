#pragma once

#include <QDialog>
#include "ui_openwithdialog.h"

namespace Utils { class FilePath; }

namespace Core {
namespace Internal {

// Present the user with a file name and a list of available
// editor kinds to choose from.
class OpenWithDialog : public QDialog, public Ui::OpenWithDialog
{
    Q_OBJECT

public:
    OpenWithDialog(const Utils::FilePath &filePath, QWidget *parent);

    void setEditors(const QStringList &);
    int editor() const;

    void setCurrentEditor(int index);

private:
    void currentItemChanged(QListWidgetItem *, QListWidgetItem *);
    void setOkButtonEnabled(bool);
};

} // namespace Internal
} // namespace Core
