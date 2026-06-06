#include "customerdialog.h"

CustomerDialog::CustomerDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Customer Management - Add New");
    setFixedSize(400, 300);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    txtName = new QLineEdit(this);
    txtPhone = new QLineEdit(this);
    txtAddress = new QLineEdit(this);

    cmbType = new QComboBox(this);
    cmbType->addItem("Farmer", clsCustomer::enCustomerType::Farmer);
    cmbType->addItem("Breeder", clsCustomer::enCustomerType::Breeder);
    cmbType->addItem("Company", clsCustomer::enCustomerType::Company);
    cmbType->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");

    formLayout->addRow("Name:", txtName);
    formLayout->addRow("Phone:", txtPhone);
    formLayout->addRow("Address:", txtAddress);
    formLayout->addRow("Customer Type:", cmbType);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton("Save Customer", this);
    btnCancel = new QPushButton("Cancel", this);

    btnSave->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px; border-radius: 4px;");

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, &CustomerDialog::handleSave);
}

void CustomerDialog::loadCustomerForEdit(const string& customerID)
{
    _currentCustomerID = customerID;
    clsCustomer customer = clsCustomer::Find(customerID);

    if (!customer.IsEmpty())
    {
        setWindowTitle("Customer Management - Edit");
        btnSave->setText("Update Customer");

        txtName->setText(QString::fromStdString(customer.FirstName()));
        txtPhone->setText(QString::fromStdString(customer.PhoneNumber()));
        txtAddress->setText(QString::fromStdString(customer.Address()));

        int index = cmbType->findData(customer.CustomerType());
        if (index != -1) cmbType->setCurrentIndex(index);
    }
}

void CustomerDialog::handleSave()
{
    if (txtName->text().isEmpty() || txtPhone->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Name and Phone are required.");
        return;
    }

    clsCustomer customerToSave = clsCustomer::GetEmptyCustomerObject();

    if (_currentCustomerID.empty()) {
        string newID = "CUST-" + to_string(clsCustomer::GetCustomersList().size() + 1);
        customerToSave = clsCustomer::GetAddNewCustomerObject(newID);
    } else {
        customerToSave = clsCustomer::Find(_currentCustomerID);
    }

    customerToSave.SetFirstName(txtName->text().toStdString());
    customerToSave.SetPhone(txtPhone->text().toStdString());
    customerToSave.SetAddress(txtAddress->text().toStdString());
    customerToSave.SetCustomerType((clsCustomer::enCustomerType)cmbType->currentData().toInt());

    customerToSave.Save();

    _saved = true;
    accept();
}
