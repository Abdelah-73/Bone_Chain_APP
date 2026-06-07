#include "customerdialog.h"

CustomerDialog::CustomerDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Customer Management - Add New"));
    setFixedSize(400, 300);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    txtName = new QLineEdit(this);
    txtPhone = new QLineEdit(this);
    txtAddress = new QLineEdit(this);

    cmbType = new QComboBox(this);
    cmbType->addItem(tr("Farmer"), clsCustomer::enCustomerType::Farmer);
    cmbType->addItem(tr("Breeder"), clsCustomer::enCustomerType::Breeder);
    cmbType->addItem(tr("Company"), clsCustomer::enCustomerType::Company);
    cmbType->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");

    txtPoints = new QLineEdit(this);
    txtPoints->setText("0");

    formLayout->addRow(tr("Name:"), txtName);
    formLayout->addRow(tr("Phone:"), txtPhone);
    formLayout->addRow(tr("Address:"), txtAddress);
    formLayout->addRow(tr("Customer Type:"), cmbType);
    formLayout->addRow(tr("Points:"), txtPoints);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton(tr("Save Customer"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

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
        setWindowTitle(tr("Customer Management - Edit"));
        btnSave->setText(tr("Update Customer"));

        txtName->setText(QString::fromStdString(customer.FirstName()));
        txtPhone->setText(QString::fromStdString(customer.PhoneNumber()));
        txtAddress->setText(QString::fromStdString(customer.Address()));
        txtPoints->setText(QString::number(customer.Points()));

        int index = cmbType->findData(customer.CustomerType());
        if (index != -1) cmbType->setCurrentIndex(index);
    }
}

void CustomerDialog::handleSave()
{
    if (txtName->text().isEmpty() || txtPhone->text().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Name and Phone are required."));
        return;
    }

    clsCustomer customerToSave = clsCustomer::GetEmptyCustomerObject();

    if (_currentCustomerID.empty()) {
        int maxNum = 0;
        for (const auto& c : clsCustomer::GetCustomersList()) {
            string cid = c.CustomerID();
            if (cid.size() > 5 && cid.substr(0, 5) == "CUST-") {
                int n = stoi(cid.substr(5));
                if (n > maxNum) maxNum = n;
            }
        }
        string newID = "CUST-" + to_string(maxNum + 1);
        customerToSave = clsCustomer::GetAddNewCustomerObject(newID);
    } else {
        customerToSave = clsCustomer::Find(_currentCustomerID);
    }

    customerToSave.SetFirstName(txtName->text().toStdString());
    customerToSave.SetPhone(txtPhone->text().toStdString());
    customerToSave.SetAddress(txtAddress->text().toStdString());
    customerToSave.SetCustomerType((clsCustomer::enCustomerType)cmbType->currentData().toInt());

    bool ok;
    int pts = txtPoints->text().toInt(&ok);
    if (ok && pts >= 0) customerToSave.SetPoints(pts);

    customerToSave.Save();

    _saved = true;
    accept();
}
