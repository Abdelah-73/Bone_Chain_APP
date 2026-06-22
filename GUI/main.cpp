#include <QApplication>
#include "mainwindow.h"
#include "ThemeManager.h"
#include "../Core/clsDatabase.h"
#include <QString>
#include <QDateTime>
#include <QTranslator>
#include <QSettings>
#include <QDir>
#include <QStringList>
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    srand(QDateTime::currentDateTime().toSecsSinceEpoch());
    QApplication app(argc, argv);

    if (!clsDatabase::GetInstance().Initialize())
    {
        std::cerr << "Failed to initialize database. Exiting.\n";
        return 1;
    }

    app.setOrganizationName("BoneChain");
    app.setApplicationName("BoneFertilizerApp");

    // Load language preference and install translator
    QSettings settings;
    QString lang = settings.value("language", "en").toString();
    QTranslator translator;
    QStringList i18nPaths = {
        QApplication::applicationDirPath() + "/../i18n",
        QApplication::applicationDirPath() + "/i18n",
        QDir::currentPath() + "/i18n"
    };
    QString qmFile = (lang == "ar") ? "bonechain_ar" : "bonechain_en";
    bool loaded = false;
    for (const QString& path : i18nPaths) {
        if (translator.load(qmFile, path)) {
            loaded = true;
            break;
        }
    }
    if (!loaded)
        (void)translator.load(qmFile);
    app.installTranslator(&translator);

    // Set layout direction based on language
    app.setLayoutDirection(lang == "ar" ? Qt::RightToLeft : Qt::LeftToRight);

    // Apply saved theme (light/dark) — loads QSS from Themes/ directory
    ThemeManager::instance().applyTheme();

    // Launch the Main Window
    MainWindow window;
    window.setWindowTitle(QObject::tr("Fertilizer OS - Enterprise Edition"));

    // Give the app a larger, modern default resolution
    window.resize(1100, 750);
    window.show();

    return app.exec();
}
