#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <string>
#include "../Core/clsProduct.h"

using namespace std;

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    ProductDialog(QWidget *parent = nullptr);
    bool wasSaved() const { return _saved; }
    void loadProductForEdit(const string& productID);

private slots:
    void handleSave();

private:
    // Product specific fields
    QLineEdit *txtName;
    QComboBox *cmbCategory;
    QLineEdit *txtPrice;
    QLineEdit *txtStockQuantity;
    QLineEdit *txtMinimumStock;
    QLineEdit *txtProductionDate;
    QLineEdit *txtExpiryDate;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool _saved = false;
    string _currentProductID = "";
};
