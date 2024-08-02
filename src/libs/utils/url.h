#pragma once

#include "utils_global.h"

QT_BEGIN_NAMESPACE
class QString;
class QUrl;
QT_END_NAMESPACE

namespace Utils {

QTCREATOR_UTILS_EXPORT QUrl urlFromLocalHostAndFreePort();
QTCREATOR_UTILS_EXPORT QUrl urlFromLocalSocket();
QTCREATOR_UTILS_EXPORT QString urlSocketScheme();
QTCREATOR_UTILS_EXPORT QString urlTcpScheme();

}
