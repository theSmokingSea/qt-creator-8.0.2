#include "environmentdialog.h"

#include "environment.h"

namespace Utils {

Utils::optional<EnvironmentItems> EnvironmentDialog::getEnvironmentItems(
    QWidget *parent, const EnvironmentItems &initial, const QString &placeholderText, Polisher polisher)
{
    return getNameValueItems(
        parent,
        initial,
        placeholderText,
        polisher,
        tr("Edit Environment"),
        tr("Enter one environment variable per line.\n"
           "To set or change a variable, use VARIABLE=VALUE.\n"
           "To append to a variable, use VARIABLE+=VALUE.\n"
           "To prepend to a variable, use VARIABLE=+VALUE.\n"
           "Existing variables can be referenced in a VALUE with ${OTHER}.\n"
           "To clear a variable, put its name on a line with nothing else on it.\n"
           "To disable a variable, prefix the line with \"#\"."));
}

} // namespace Utils
