#pragma once

#include "utils_global.h"

#include <QDialog>

namespace Utils {

class FilePath;
namespace Ui { class RemoveFileDialog; }

class QTCREATOR_UTILS_EXPORT RemoveFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveFileDialog(const FilePath &filePath, QWidget *parent = nullptr);
    ~RemoveFileDialog() override;

    void setDeleteFileVisible(bool visible);
    bool isDeleteFileChecked() const;

private:
    Ui::RemoveFileDialog *m_ui;
};

} // namespace Utils
