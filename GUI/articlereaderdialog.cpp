#include "articlereaderdialog.h"

ArticleReaderDialog::ArticleReaderDialog(const string& articleID, QWidget *parent) : QDialog(parent)
{
    // 1. Fetch the Article from the Backend
    clsArticle article = clsArticle::Find(articleID);

    // 2. Setup the Window
    setWindowTitle("Reader - " + QString::fromStdString(article.Title()));
    setFixedSize(650, 750); // Tall and wide enough for comfortable reading
    setStyleSheet("background-color: #f4f6f9; color: #2c3e50;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 25, 25, 25);

    // 3. Create Typography Elements
    QLabel *lblTitle = new QLabel(QString::fromStdString(article.Title()), this);
    lblTitle->setStyleSheet("font-size: 26px; font-weight: bold; color: #2980b9;");
    lblTitle->setWordWrap(true); // Allow long titles to wrap to the next line

    QLabel *lblMeta = new QLabel(QString::fromStdString(article.Category() + "  |  Published: " + article.PublishDate()), this);
    lblMeta->setStyleSheet("font-size: 13px; color: #7f8c8d; font-style: italic;");

    // 4. The Main Reading Area
    QTextBrowser *txtContent = new QTextBrowser(this);
    txtContent->setPlainText(QString::fromStdString(article.Content()));

    // Style the reader area to look like a clean piece of paper
    txtContent->setStyleSheet(R"(
        QTextBrowser {
            background-color: #ffffff;
            border: 1px solid #bdc3c7;
            border-radius: 8px;
            padding: 20px;
            font-size: 16px;
            line-height: 1.8;
            color: #34495e;
        }
    )");

    // 5. Close Button
    QPushButton *btnClose = new QPushButton("Close Article", this);
    btnClose->setFixedSize(150, 40);
    btnClose->setStyleSheet("background-color: #34495e; color: white; border-radius: 6px; font-weight: bold;");
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    // 6. Assemble the Layout
    layout->addWidget(lblTitle);
    layout->addWidget(lblMeta);
    layout->addWidget(txtContent, 1); // The '1' makes the text box stretch to fill space
    layout->addWidget(btnClose, 0, Qt::AlignCenter);
}
