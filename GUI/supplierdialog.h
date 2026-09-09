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
#include "../Core/clsSupplier.h"

using namespace std;

class SupplierDialog : public QDialog
{
    Q_OBJECT

public:
    SupplierDialog(QWidget *parent = nullptr);
    bool wasSaved() const { return _saved; }
    void loadSupplierForEdit(const string& supplierID);

private slots:
    void handleSave();

private:
    // Supplier specific fields
    QLineEdit *txtName;
    QLineEdit *txtPhone;
    QLineEdit *txtAddress;
    QComboBox *cmbType;
    QLineEdit *txtBoneType;
    QLineEdit *txtWeeklyQuantity;
    QLineEdit *txtPoints;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool _saved = false;
    string _currentSupplierID = "";
};
