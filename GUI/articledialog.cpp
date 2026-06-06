#include "articledialog.h"

ArticleDialog::ArticleDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Article Publisher - Create New");
    setFixedSize(500, 550); // Made slightly wider and taller for text editing
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Initialize Fields
    txtTitle = new QLineEdit(this);

    // Categories for the Articles
    cmbCategory = new QComboBox(this);
    cmbCategory->addItems({"Agriculture Guides", "Industry News", "Company Updates", "Research & Science"});
    cmbCategory->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");

    txtContent = new QTextEdit(this);
    txtContent->setPlaceholderText("Write your article content here...");
    txtContent->setMinimumHeight(250); // Give the text area plenty of room
    txtContent->setStyleSheet("border: 2px solid #dfe6e9; border-radius: 8px; padding: 5px;");

    txtPublishDate = new QLineEdit(this);
    txtPublishDate->setPlaceholderText("YYYY-MM-DD");

    formLayout->addRow("Title:", txtTitle);
    formLayout->addRow("Category:", cmbCategory);
    formLayout->addRow("Date:", txtPublishDate);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton("Publish Article", this);
    btnCancel = new QPushButton("Cancel", this);

    btnSave->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px; border-radius: 4px;");

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);

    mainLayout->addLayout(formLayout);

    // Add the content box below the form layout so it stretches fully
    QLabel *lblContent = new QLabel("Article Content:", this);
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
        setWindowTitle("Article Publisher - Edit");
        btnSave->setText("Update Article");

        txtTitle->setText(QString::fromStdString(article.Title()));
        cmbCategory->setCurrentText(QString::fromStdString(article.Category()));
        txtContent->setPlainText(QString::fromStdString(article.Content()));
        txtPublishDate->setText(QString::fromStdString(article.PublishDate()));
    }
}

void ArticleDialog::handleSave()
{
    if (txtTitle->text().isEmpty() || txtContent->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "A Title and Content are required to publish.");
        return;
    }

    clsArticle articleToSave = clsArticle::GetEmptyArticleObject();

    if (_currentArticleID.empty()) {
        string newID = "ART-" + to_string(clsArticle::GetArticlesList().size() + 1);
        articleToSave = clsArticle::GetAddNewArticleObject(newID);
    } else {
        articleToSave = clsArticle::Find(_currentArticleID);
    }

    // Apply data (Note the use of toPlainText() for the QTextEdit)
    articleToSave.SetTitle(txtTitle->text().toStdString());
    articleToSave.SetCategory(cmbCategory->currentText().toStdString());
    articleToSave.SetContent(txtContent->toPlainText().toStdString());
    articleToSave.SetPublishDate(txtPublishDate->text().toStdString());

    articleToSave.Save();

    _saved = true;
    accept();
}
