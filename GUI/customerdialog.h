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
#include "../Core/clsCustomer.h"

using namespace std;

class CustomerDialog : public QDialog
{
    Q_OBJECT

public:
    CustomerDialog(QWidget *parent = nullptr);
    bool wasSaved() const { return _saved; }
    void loadCustomerForEdit(const string& customerID);

private slots:
    void handleSave();

private:
    QLineEdit *txtName;
    QLineEdit *txtPhone;
    QLineEdit *txtAddress;
    QComboBox *cmbType;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool _saved = false;
    string _currentCustomerID = "";
};
