#include "userdialog.h"
#include <QDateTime>

UserDialog::UserDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("User Management - Add User");
    setFixedSize(400, 500);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    txtFirstName = new QLineEdit(this);
    txtLastName = new QLineEdit(this);
    txtEmail = new QLineEdit(this);
    txtPhone = new QLineEdit(this);
    txtUsername = new QLineEdit(this);
    txtPassword = new QLineEdit(this);
    txtPassword->setEchoMode(QLineEdit::Password);

    cmbRole = new QComboBox(this);
    cmbRole->addItem("Admin", clsUser::enRole::Admin);
    cmbRole->addItem("Supplier", clsUser::enRole::Supplier);
    cmbRole->addItem("Customer", clsUser::enRole::Customer);
    cmbRole->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");

    chkIsActive = new QCheckBox("Account is Active", this);
    chkIsActive->setChecked(true);

    formLayout->addRow("First Name:", txtFirstName);
    formLayout->addRow("Last Name:", txtLastName);
    formLayout->addRow("Email:", txtEmail);
    formLayout->addRow("Phone:", txtPhone);
    formLayout->addRow("Username:", txtUsername);
    formLayout->addRow("Password:", txtPassword);
    formLayout->addRow("Role:", cmbRole);
    formLayout->addRow("", chkIsActive);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton("Save User", this);
    btnCancel = new QPushButton("Cancel", this);

    btnSave->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px; border-radius: 4px;");

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, &UserDialog::handleSave);
}

void UserDialog::loadUserForEdit(const string& userID)
{
    _currentUserID = userID;

    clsUser user = clsUser::Find(userID);
    if (!user.IsEmpty())
    {
        setWindowTitle("User Management - Edit User");
        btnSave->setText("Update User");

        txtFirstName->setText(QString::fromStdString(user.FirstName()));
        txtLastName->setText(QString::fromStdString(user.LastName()));
        txtEmail->setText(QString::fromStdString(user.Email()));
        txtPhone->setText(QString::fromStdString(user.PhoneNumber()));
        txtUsername->setText(QString::fromStdString(user.Username()));
        txtPassword->setText(QString::fromStdString(user.Password()));

        int index = cmbRole->findData(user.Role());
        if (index != -1) cmbRole->setCurrentIndex(index);

        chkIsActive->setChecked(user.IsActive());
    }
}

void UserDialog::handleSave()
{
    if (txtUsername->text().isEmpty() || txtPassword->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Username and Password are required.");
        return;
    }

    clsUser userToSave = clsUser::GetEmptyUserObject();

    if (_currentUserID.empty())
    {
        vector<clsUser> existingUsers = clsUser::GetUsersList();
        int maxNum = 0;
        for (clsUser& u : existingUsers) {
            string uid = u.UserID();
            if (uid.size() > 4 && uid.substr(0, 4) == "USR-") {
                int num = stoi(uid.substr(4));
                maxNum = max(maxNum, num);
            }
        }
        string newID = "USR-" + to_string(maxNum + 1);
        userToSave = clsUser::GetAddNewUserObject(newID);
    }
    else
    {
        userToSave = clsUser::Find(_currentUserID);
    }

    if (_currentUserID.empty()) {
        vector<clsUser> existingUsers = clsUser::GetUsersList();
        for (clsUser& u : existingUsers) {
            if (u.Username() == txtUsername->text().toStdString()) {
                QMessageBox::warning(this, "Validation Error", "Username already exists. Please choose another.");
                return;
            }
        }
    }

    userToSave.SetFirstName(txtFirstName->text().toStdString());
    userToSave.SetLastName(txtLastName->text().toStdString());
    userToSave.SetEmail(txtEmail->text().toStdString());
    userToSave.SetPhone(txtPhone->text().toStdString());
    userToSave.SetUsername(txtUsername->text().toStdString());
    userToSave.SetPassword(txtPassword->text().toStdString());
    userToSave.SetRole(cmbRole->currentData().toInt());
    userToSave.SetIsActive(chkIsActive->isChecked());

    if (userToSave.Role() == clsUser::enRole::Supplier) {
        vector<clsSupplier> existingSuppliers = clsSupplier::GetSuppliersList();
        int maxNum = 0;
        for (clsSupplier& s : existingSuppliers) {
            string sid = s.SupplierID();
            if (sid.size() > 4 && sid.substr(0, 4) == "SUP-") {
                int num = stoi(sid.substr(4));
                maxNum = max(maxNum, num);
            }
        }
        string newSupID = "SUP-" + to_string(maxNum + 1);
        clsSupplier::CreateNewSupplier(newSupID,
            txtFirstName->text().toStdString(),
            txtPhone->text().toStdString());
        userToSave.SetSupplierID(newSupID);
    }

    if (userToSave.Role() == clsUser::enRole::Customer) {
        vector<clsCustomer> existingCustomers = clsCustomer::GetCustomersList();
        int maxNum = 0;
        for (clsCustomer& c : existingCustomers) {
            string cid = c.CustomerID();
            if (cid.size() > 5 && cid.substr(0, 5) == "CUST-") {
                int num = stoi(cid.substr(5));
                maxNum = max(maxNum, num);
            }
        }
        string newCustID = "CUST-" + to_string(maxNum + 1);
        clsCustomer::CreateNewCustomer(newCustID,
            txtFirstName->text().toStdString(),
            txtPhone->text().toStdString());
        userToSave.SetCustomerID(newCustID);
    }

    userToSave.Save();

    _saved = true;
    accept();
}
