#include "articledialog.h"

ArticleDialog::ArticleDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Article Publisher - Create New"));
    setFixedSize(500, 550);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Initialize Fields
    txtTitle = new QLineEdit(this);

    // Categories for the Articles
    cmbCategory = new QComboBox(this);
    cmbCategory->addItem(tr("Agriculture Guides"));
    cmbCategory->addItem(tr("Industry News"));
    cmbCategory->addItem(tr("Company Updates"));
    cmbCategory->addItem(tr("Research & Science"));
    cmbCategory->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");

    txtContent = new QTextEdit(this);
    txtContent->setPlaceholderText(tr("Write your article content here..."));
    txtContent->setMinimumHeight(250);
    txtContent->setStyleSheet("border: 2px solid #dfe6e9; border-radius: 8px; padding: 5px;");

    txtPublishDate = new QLineEdit(this);
    txtPublishDate->setPlaceholderText(tr("YYYY-MM-DD"));

    formLayout->addRow(tr("Title:"), txtTitle);
    formLayout->addRow(tr("Category:"), cmbCategory);
    formLayout->addRow(tr("Date:"), txtPublishDate);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton(tr("Publish Article"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

    btnSave->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px; border-radius: 4px;");

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);

    mainLayout->addLayout(formLayout);

    // Add the content box below the form layout so it stretches fully
    QLabel *lblContent = new QLabel(tr("Article Content:"), this);
    lblContent->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(lblContent);
    mainLayout->addWidget(txtContent);

    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, &ArticleDialog::handleSave);
}

void ArticleDialog::loadArticleForEdit(const string& articleID)
{
    _currentArticleID = articleID;
    clsArticle article = clsArticle::Find(articleID);

    if (!article.IsEmpty())
    {
        setWindowTitle(tr("Article Publisher - Edit"));
        btnSave->setText(tr("Update Article"));

        txtTitle->setText(QString::fromStdString(article.Title()));
        cmbCategory->setCurrentText(QString::fromStdString(article.Category()));
        txtContent->setPlainText(QString::fromStdString(article.Content()));
        txtPublishDate->setText(QString::fromStdString(article.PublishDate()));
    }
}

void ArticleDialog::handleSave()
{
    if (txtTitle->text().isEmpty() || txtContent->toPlainText().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("A Title and Content are required to publish."));
        return;
    }

    clsArticle articleToSave = clsArticle::GetEmptyArticleObject();

    if (_currentArticleID.empty()) {
        int maxNum = 0;
        for (const auto& a : clsArticle::GetArticlesList()) {
            string id = a.ArticleID();
            if (id.rfind("ART-", 0) == 0) {
                try { int n = stoi(id.substr(4)); if (n > maxNum) maxNum = n; }
                catch (...) { }
            }
        }
        string newID = "ART-" + to_string(maxNum + 1);
        articleToSave = clsArticle::GetAddNewArticleObject(newID);
    } else {
        articleToSave = clsArticle::Find(_currentArticleID);
    }

    // Apply data
    articleToSave.SetTitle(txtTitle->text().toStdString());
    articleToSave.SetCategory(cmbCategory->currentText().toStdString());
    articleToSave.SetContent(txtContent->toPlainText().toStdString());
    articleToSave.SetPublishDate(txtPublishDate->text().toStdString());

    articleToSave.Save();

    _saved = true;
    accept();
}
