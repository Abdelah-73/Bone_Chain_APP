#include "ThemeManager.h"
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QDir>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager()
    : _currentTheme("light")
{
    QSettings settings;
    _currentTheme = settings.value("theme", "light").toString();
}

void ThemeManager::loadTheme(const QString& themeName)
{
    if (themeName == _currentTheme) return;
    _currentTheme = themeName;
    applyTheme();
}

void ThemeManager::applyTheme()
{
    QString qssPath = findQssFile(_currentTheme);
    if (qssPath.isEmpty()) {
        qWarning("ThemeManager: QSS file not found for theme '%s'", qPrintable(_currentTheme));
        return;
    }

    QString qss = loadQssFile(qssPath);
    if (qss.isEmpty()) {
        qWarning("ThemeManager: Failed to load QSS file: %s", qPrintable(qssPath));
        return;
    }

    qApp->setStyleSheet(qss);

    QSettings settings;
    settings.setValue("theme", _currentTheme);

    emit themeChanged(_currentTheme);
}

QString ThemeManager::findQssFile(const QString& themeName) const
{
    QString fileName = themeName + ".qss";

    QStringList searchPaths = {
        QApplication::applicationDirPath() + "/../Themes",
        QApplication::applicationDirPath() + "/Themes",
        QDir::currentPath() + "/Themes"
    };

    for (const QString& dirPath : searchPaths) {
        QString fullPath = dirPath + "/" + fileName;
        if (QFile::exists(fullPath))
            return fullPath;
    }

    return QString();
}

QString ThemeManager::loadQssFile(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString content = file.readAll();
    file.close();
    return content;
}
