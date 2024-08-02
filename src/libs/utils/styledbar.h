#pragma once

#include "utils_global.h"

#include <QWidget>

namespace Utils {

class QTCREATOR_UTILS_EXPORT StyledBar : public QWidget
{
    Q_OBJECT
public:
    StyledBar(QWidget *parent = nullptr);
    void setSingleRow(bool singleRow);
    bool isSingleRow() const;

    void setLightColored(bool lightColored);
    bool isLightColored() const;

protected:
    void paintEvent(QPaintEvent *event) override;
};

class QTCREATOR_UTILS_EXPORT StyledSeparator : public QWidget
{
    Q_OBJECT
public:
    StyledSeparator(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};

} // Utils
