#pragma once
#include <QDialog>
#include <QTextBrowser>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <string>
#include "../Core/clsArticle.h"

using namespace std;

class ArticleReaderDialog : public QDialog
{
    Q_OBJECT

public:
    // We pass the ID directly into the constructor
    ArticleReaderDialog(const string& articleID, QWidget *parent = nullptr);
};
