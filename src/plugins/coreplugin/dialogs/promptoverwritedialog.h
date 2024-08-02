#pragma once

#include "../core_global.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTreeView;
class QStandardItemModel;
class QStandardItem;
class QLabel;
QT_END_NAMESPACE

namespace Core {

// Documentation inside.
class CORE_EXPORT PromptOverwriteDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PromptOverwriteDialog(QWidget *parent = nullptr);

    void setFiles(const QStringList &);

    void setFileEnabled(const QString &f, bool e);
    bool isFileEnabled(const QString &f) const;

    void setFileChecked(const QString &f, bool e);
    bool isFileChecked(const QString &f) const;

    QStringList checkedFiles() const   { return files(Qt::Checked); }
    QStringList uncheckedFiles() const { return files(Qt::Unchecked); }

private:
    QStandardItem *itemForFile(const QString &f) const;
    QStringList files(Qt::CheckState cs) const;

    QLabel *m_label;
    QTreeView *m_view;
    QStandardItemModel *m_model;
};

} // namespace Core
