#pragma once

#include <QAbstractItemView>

namespace Core {
namespace Internal {
namespace ItemDataRoles {

enum Roles {
    ResultItemRole = Qt::UserRole,
    ResultLineRole,
    ResultBeginLineNumberRole,
    ResultIconRole,
    ResultHighlightBackgroundColor,
    ResultHighlightForegroundColor,
    FunctionHighlightBackgroundColor,
    FunctionHighlightForegroundColor,
    ResultBeginColumnNumberRole,
    SearchTermLengthRole,
    ContainingFunctionNameRole,
    IsGeneratedRole
};

} // namespace Internal
} // namespace Core
} // namespace ItemDataRoles
