#pragma once

#include <coreplugin/core_global.h>

#include <utils/id.h>

#include <QMap>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT OptionsPopup : public QWidget
{
    Q_OBJECT

public:
    OptionsPopup(QWidget *parent, const QVector<Utils::Id> &commands);

protected:
    bool event(QEvent *ev) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    void actionChanged();

    QCheckBox *createCheckboxForCommand(Utils::Id id);

    QMap<QAction *, QCheckBox *> m_checkboxMap;
};

} // namespace Core
