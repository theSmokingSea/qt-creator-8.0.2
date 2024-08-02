#pragma once

#include "../iwizardfactory.h"
#include "newdialog.h"

#include <QDialog>
#include <QIcon>
#include <QList>
#include <QVariantMap>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QSortFilterProxyModel;
class QPushButton;
class QStandardItem;
class QStandardItemModel;
QT_END_NAMESPACE

namespace Core {

namespace Internal {

namespace Ui { class NewDialog; }

class NewDialogWidget : public QDialog, public NewDialog
{
    Q_OBJECT

public:
    explicit NewDialogWidget(QWidget *parent);
    ~NewDialogWidget() override;

    void setWizardFactories(QList<IWizardFactory *> factories,
                            const Utils::FilePath &defaultLocation,
                            const QVariantMap &extraVariables) override;

    void showDialog() override;
    Utils::Id selectedPlatform() const;
    QWidget *widget() override { return this; }

    void setWindowTitle(const QString &title) override {
        QDialog::setWindowTitle(title);
    }

protected:
    bool event(QEvent *) override;

private:
    void currentCategoryChanged(const QModelIndex &);
    void currentItemChanged(const QModelIndex &);
    void accept() override;
    void reject() override;
    void updateOkButton();
    void setSelectedPlatform(int index);

    Core::IWizardFactory *currentWizardFactory() const;
    void addItem(QStandardItem *topLevelCategoryItem, IWizardFactory *factory);
    void saveState();

    Ui::NewDialog *m_ui;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *m_filterProxyModel;
    QPushButton *m_okButton = nullptr;
    QList<QStandardItem *> m_categoryItems;
    Utils::FilePath m_defaultLocation;
    QVariantMap m_extraVariables;
};

} // namespace Internal
} // namespace Core
