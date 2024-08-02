#pragma once

#include <utils/id.h>

#include <coreplugin/dialogs/ioptionspage.h>

#include <QWidget>

namespace Utils { class Theme; }

namespace Core {
namespace Internal {

class ThemeChooserPrivate;

class ThemeEntry
{
public:
    ThemeEntry() = default;
    ThemeEntry(Utils::Id id, const QString &filePath);

    Utils::Id id() const;
    QString displayName() const;
    QString filePath() const;
    static QList<ThemeEntry> availableThemes();
    static Utils::Id themeSetting();
    static Utils::Theme *createTheme(Utils::Id id);

private:
    Utils::Id m_id;
    QString m_filePath;
    mutable QString m_displayName;
};

class ThemeChooser : public QWidget
{
    Q_OBJECT

public:
    ThemeChooser(QWidget *parent = nullptr);
    ~ThemeChooser() override;

    void apply();

private:
    ThemeChooserPrivate *d;
};

} // namespace Internal
} // namespace Core
