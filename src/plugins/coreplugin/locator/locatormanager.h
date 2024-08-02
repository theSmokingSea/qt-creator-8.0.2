#pragma once

#include <coreplugin/core_global.h>

#include <QObject>

namespace Core {

class ILocatorFilter;

class CORE_EXPORT LocatorManager : public QObject
{
    Q_OBJECT

public:
    LocatorManager();

    static void showFilter(ILocatorFilter *filter);
    static void show(const QString &text, int selectionStart = -1, int selectionLength = 0);

    static QWidget *createLocatorInputWidget(QWidget *window);

    static bool locatorHasFocus();
};

} // namespace Core
