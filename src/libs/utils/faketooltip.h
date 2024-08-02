#pragma once

#include "utils_global.h"

#include <QWidget>

namespace Utils {

class QTCREATOR_UTILS_EXPORT FakeToolTip : public QWidget
{
    Q_OBJECT

public:
    explicit FakeToolTip(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
};

} // namespace Utils
