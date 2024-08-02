#pragma once

#include "utils_global.h"

#include "namevalueitem.h"
#include "optional.h"

#include <QDialog>

#include <functional>
#include <memory>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Utils {

namespace Internal {
class NameValueItemsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NameValueItemsWidget(QWidget *parent = nullptr);

    void setEnvironmentItems(const EnvironmentItems &items);
    EnvironmentItems environmentItems() const;

    void setPlaceholderText(const QString &text);

private:
    QPlainTextEdit *m_editor;
};
} // namespace Internal

class QTCREATOR_UTILS_EXPORT NameValuesDialog : public QDialog
{
    Q_OBJECT
public:
    void setNameValueItems(const NameValueItems &items);
    NameValueItems nameValueItems() const;

    void setPlaceholderText(const QString &text);

    using Polisher = std::function<void(QWidget *)>;
    static Utils::optional<NameValueItems> getNameValueItems(QWidget *parent = nullptr,
                                                             const NameValueItems &initial = {},
                                                             const QString &placeholderText = {},
                                                             Polisher polish = {},
                                                             const QString &windowTitle = {},
                                                             const QString &helpText = {});

protected:
    explicit NameValuesDialog(const QString &windowTitle,
                              const QString &helpText,
                              QWidget *parent = {});

private:
    Internal::NameValueItemsWidget *m_editor;
};

} // namespace Utils
