#pragma once

#include <QVector>

QT_BEGIN_NAMESPACE
class QTreeView;
QT_END_NAMESPACE

namespace Utils {
class NameValueDictionary;
class NameValueItem;
using NameValueItems = QVector<NameValueItem>;

class Environment;
using EnvironmentItem = NameValueItem;
using EnvironmentItems = NameValueItems;

class PreprocessorMacroDictionary;
using PreprocessorMacroItem = NameValueItem;
using PreprocessorMacroItems = NameValueItems;

class NameValueModel;
class EnvironmentModel;
} // namespace Utils
