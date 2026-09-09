#pragma once
#include <QObject>
#include <QString>
#include <QStringList>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    static ThemeManager& instance();

    void loadTheme(const QString& themeName);
    void applyTheme();
    QString currentTheme() const { return _currentTheme; }
    QStringList availableThemes() const { return {"light", "dark"}; }

signals:
    void themeChanged(const QString& themeName);

private:
    ThemeManager();
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    QString findQssFile(const QString& themeName) const;
    QString loadQssFile(const QString& filePath) const;
    QString _currentTheme;
};
