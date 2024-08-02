#pragma once

#include "myinterfaces.h"
#include "ui_main.h"

#include <aggregate.h>

#include <QWidget>

class MyMain : public QWidget
{
    Q_OBJECT

public:
    explicit MyMain(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MyMain();

    void add(IComboEntry *obj);

private:
    void select(int index);

    Ui::mainClass ui;

    QList<IComboEntry *> m_entries;
};
