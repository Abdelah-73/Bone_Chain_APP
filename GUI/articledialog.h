#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <string>
#include "../Core/clsArticle.h"

using namespace std;

class ArticleDialog : public QDialog
{
    Q_OBJECT

public:
    ArticleDialog(QWidget *parent = nullptr);
    bool wasSaved() const { return _saved; }
    void loadArticleForEdit(const string& articleID);

private slots:
    void handleSave();

private:
    QLineEdit *txtTitle;
    QComboBox *cmbCategory;
    QTextEdit *txtContent; // Multi-line text area for the article body
    QLineEdit *txtPublishDate;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool _saved = false;
    string _currentArticleID = "";
};
