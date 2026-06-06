#include "userdialog.h"

UserDialog::UserDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("User Management - Add User");
    setFixedSize(400, 500);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Initialize Fields
    txtFirstName = new QLineEdit(this);
    txtLastName = new QLineEdit(this);
    txtEmail = new QLineEdit(this);
    txtPhone = new QLineEdit(this);
    txtUsername = new QLineEdit(this);
    txtPassword = new QLineEdit(this);
    txtPassword->setEchoMode(QLineEdit::Password);

    // Initialize Roles (Admin, Supplier, Customer)
    cmbRole = new QComboBox(this);
    cmbRole->addItem("Admin", clsUser::enRole::Admin);
    cmbRole->addItem("Supplier", clsUser::enRole::Supplier);
    cmbRole->addItem("Customer", clsUser::enRole::Customer);
    cmbRole->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");

    // Initialize Status
    chkIsActive = new QCheckBox("Account is Active", this);
    chkIsActive->setChecked(true); // Default to active

    // Add fields to form
    formLayout->addRow("First Name:", txtFirstName);
    formLayout->addRow("Last Name:", txtLastName);
    formLayout->addRow("Email:", txtEmail);
    formLayout->addRow("Phone:", txtPhone);
    formLayout->addRow("Username:", txtUsername);
    formLayout->addRow("Password:", txtPassword);
    formLayout->addRow("Role:", cmbRole);
    formLayout->addRow("", chkIsActive);

    // Setup Buttons
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
    _currentUserID = userID; // Save the ID so the system knows we are in Update Mode

    clsUser user = clsUser::Find(userID);
    if (!user.IsEmpty())
    {
        setWindowTitle("User Management - Edit User"); // Change the window title
        btnSave->setText("Update User"); // Change button text

        // Pre-fill the text boxes
        txtFirstName->setText(QString::fromStdString(user.FirstName()));
        txtLastName->setText(QString::fromStdString(user.LastName()));
        txtEmail->setText(QString::fromStdString(user.Email()));
        txtPhone->setText(QString::fromStdString(user.PhoneNumber()));
        txtUsername->setText(QString::fromStdString(user.Username()));
        txtPassword->setText(QString::fromStdString(user.Password()));

        // Set the dropdown to the correct role
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

    // Prepare a blank user object
    clsUser userToSave = clsUser::GetEmptyUserObject();

    if (_currentUserID.empty())
    {
        // ADD NEW MODE
        string newID = "USR-" + to_string(clsUser::GetUsersList().size() + 1);
        userToSave = clsUser::GetAddNewUserObject(newID);
    }
    else
    {
        // UPDATE MODE
        userToSave = clsUser::Find(_currentUserID);
    }

    // Apply the data from the form to the object
    userToSave.SetFirstName(txtFirstName->text().toStdString());
    userToSave.SetLastName(txtLastName->text().toStdString());
    userToSave.SetEmail(txtEmail->text().toStdString());
    userToSave.SetPhone(txtPhone->text().toStdString());
    userToSave.SetUsername(txtUsername->text().toStdString());
    userToSave.SetPassword(txtPassword->text().toStdString());
    userToSave.SetRole(cmbRole->currentData().toInt());
    userToSave.SetIsActive(chkIsActive->isChecked());

    // Execute the backend save (it automatically knows whether to Add or Update based on its internal mode!)
    userToSave.Save();

    _saved = true;
    accept(); // Close the dialog
}
