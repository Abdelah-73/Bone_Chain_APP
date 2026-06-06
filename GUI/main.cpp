#include <QApplication>
#include "mainwindow.h"
#include <QString>
#include <QDateTime>
#include <cstdlib>

int main(int argc, char *argv[])
{
    srand(QDateTime::currentDateTime().toSecsSinceEpoch());
    QApplication app(argc, argv);

    // 1. Define the Global Qt Style Sheet (QSS)
    QString modernTheme = R"(
        /* --- Global Application Background & Typography --- */
        QMainWindow {
            background-color: #f4f6f9;
        }
        QWidget {
            font-family: 'Segoe UI', 'Helvetica Neue', Arial, sans-serif;
            color: #2c3e50;
        }

        /* --- Input Fields (Login & Forms) --- */
        QLineEdit {
            background-color: #ffffff;
            border: 2px solid #dfe6e9;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            color: #2d3436;
        }
        QLineEdit:focus {
            border: 2px solid #3498db;
            background-color: #fcfcfc;
        }

        /* --- Standard Action Buttons --- */
        QPushButton {
            background-color: #3498db;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #1f618d;
        }

        /* --- Danger Buttons (Delete / Logout) --- */
        QPushButton[text="Delete Selected"],
        QPushButton[text="Delete User"],
        QPushButton[text="Delete Supplier"],
        QPushButton[text="Delete Customer"],
        QPushButton[text="Delete Product"],
        QPushButton[text="Cancel Order"],
        QPushButton[text="Delete Article"] {
            background-color: #e74c3c;
        }
        QPushButton[text*="Delete"]:hover, QPushButton[text*="Cancel"]:hover {
            background-color: #c0392b;
        }

        /* --- Data Grids (Tables) --- */
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f8f9fa;
            border: 1px solid #e0e0e0;
            border-radius: 8px;
            gridline-color: #ecf0f1;
            selection-background-color: #ebf5fb;
            selection-color: #2980b9;
            font-size: 13px;
        }
        QHeaderView::section {
            background-color: #ecf0f1;
            color: #34495e;
            padding: 10px;
            border: none;
            border-bottom: 2px solid #bdc3c7;
            font-weight: bold;
            font-size: 13px;
        }
        QTableWidget::item {
            padding: 5px;
            border: none;
        }
    )";

    // 2. Apply the theme globally to the entire application engine
    app.setStyleSheet(modernTheme);

    // 3. Launch the Main Window
    MainWindow window;
    window.setWindowTitle("Fertilizer OS - Enterprise Edition");

    // Give the app a larger, modern default resolution
    window.resize(1100, 750);
    window.show();

    return app.exec();
}
