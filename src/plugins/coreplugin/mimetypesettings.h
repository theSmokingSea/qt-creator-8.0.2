#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

namespace Core {
namespace Internal {

class MimeTypeSettingsPrivate;

class MimeTypeSettings : public IOptionsPage
{
    Q_OBJECT

public:
    MimeTypeSettings();
    ~MimeTypeSettings() override;

    QWidget *widget() override;
    void apply() override;
    void finish() override;

    static void restoreSettings();
private:
    MimeTypeSettingsPrivate *d;
};

} // Internal
} // Core
