#pragma once

#include <utils/filepath.h>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class FilePropertiesDialog; }
QT_END_NAMESPACE

namespace Core {

class FilePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePropertiesDialog(const Utils::FilePath &filePath, QWidget *parent = nullptr);
    ~FilePropertiesDialog() override;

private:
    void refresh();
    void setPermission(QFile::Permissions newPermissions, bool set);
    void detectTextFileSettings();

private:
    Ui::FilePropertiesDialog *m_ui = nullptr;
    const Utils::FilePath m_filePath;
};

} // Core
