#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <string>
#include "../Core/clsUser.h"
#include "../Core/clsSupplier.h"
#include "../Core/clsCustomer.h"

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    UserDialog(QWidget *parent = nullptr);
    bool wasSaved() const { return _saved; }

    void loadUserForEdit(const string& userID);

private slots:
    void handleSave();

private:
    // User Data Fields
    QLineEdit *txtFirstName;
    QLineEdit *txtLastName;
    QLineEdit *txtEmail;
    QLineEdit *txtPhone;
    QLineEdit *txtUsername;
    QLineEdit *txtPassword;

    // Roles & Status
    QComboBox *cmbRole;
    QCheckBox *chkIsActive;

    // Action Buttons
    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool _saved = false;

    string _currentUserID = "";
};
