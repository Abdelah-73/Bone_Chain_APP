#include "supplierdialog.h"

SupplierDialog::SupplierDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Supplier Management - Add New"));
    setFixedSize(400, 450);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Initialize Fields
    txtName = new QLineEdit(this);
    txtPhone = new QLineEdit(this);
    txtAddress = new QLineEdit(this);

    // Initialize Type (Butcher, Restaurant, Individual)
    cmbType = new QComboBox(this);
    cmbType->addItem(tr("Butcher"), clsSupplier::enSupplierType::Butcher);
    cmbType->addItem(tr("Restaurant"), clsSupplier::enSupplierType::Restaurant);
    cmbType->addItem(tr("Individual"), clsSupplier::enSupplierType::Individual);
    cmbType->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");

    txtBoneType = new QLineEdit(this);
    txtWeeklyQuantity = new QLineEdit(this);
    txtWeeklyQuantity->setPlaceholderText(tr("0.0"));
    txtPoints = new QLineEdit(this);
    txtPoints->setText("0");

    // Add to form
    formLayout->addRow(tr("Name:"), txtName);
    formLayout->addRow(tr("Phone:"), txtPhone);
    formLayout->addRow(tr("Address:"), txtAddress);
    formLayout->addRow(tr("Type:"), cmbType);
    formLayout->addRow(tr("Primary Bone Type:"), txtBoneType);
    formLayout->addRow(tr("Avg. Weekly Qty (kg):"), txtWeeklyQuantity);
    formLayout->addRow(tr("Reward Points:"), txtPoints);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton(tr("Save Supplier"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

    btnSave->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px; border-radius: 4px;");

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, &SupplierDialog::handleSave);
}

void SupplierDialog::loadSupplierForEdit(const string& supplierID)
{
    _currentSupplierID = supplierID;
    clsSupplier supplier = clsSupplier::Find(supplierID);

    if (!supplier.IsEmpty())
    {
        setWindowTitle(tr("Supplier Management - Edit"));
        btnSave->setText(tr("Update Supplier"));

        // Pre-fill existing data
        txtName->setText(QString::fromStdString(supplier.FirstName()));
        txtPhone->setText(QString::fromStdString(supplier.PhoneNumber()));
        txtAddress->setText(QString::fromStdString(supplier.Address()));

        int index = cmbType->findData(supplier.SupplierType());
        if (index != -1) cmbType->setCurrentIndex(index);

        txtBoneType->setText(QString::fromStdString(supplier.BoneType()));
        txtWeeklyQuantity->setText(QString::number(supplier.WeeklyQuantity()));
        txtPoints->setText(QString::number(supplier.Points()));
    }
}

void SupplierDialog::handleSave()
{
    if (txtName->text().isEmpty() || txtPhone->text().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Name and Phone are required."));
        return;
    }

    clsSupplier supplierToSave = clsSupplier::GetEmptySupplierObject();

    if (_currentSupplierID.empty()) {
        // Add Mode
        vector<clsSupplier> existing = clsSupplier::GetSuppliersList();
        int maxNum = 0;
        for (clsSupplier& s : existing) {
            string sid = s.SupplierID();
            if (sid.size() > 4 && sid.substr(0, 4) == "SUP-") {
                int num = stoi(sid.substr(4));
                if (num > maxNum) maxNum = num;
            }
        }
        string newID = "SUP-" + to_string(maxNum + 1);
        supplierToSave = clsSupplier::GetAddNewSupplierObject(newID);
    } else {
        // Update Mode
        supplierToSave = clsSupplier::Find(_currentSupplierID);
    }

    bool ok;
    double weeklyQty = txtWeeklyQuantity->text().toDouble(&ok);
    if (!ok || weeklyQty < 0) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Weekly Quantity must be a valid positive number."));
        return;
    }

    int points = txtPoints->text().toInt(&ok);
    if (!ok || points < 0) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Points must be a valid non-negative integer."));
        return;
    }

    supplierToSave.SetFirstName(txtName->text().toStdString());
    supplierToSave.SetPhone(txtPhone->text().toStdString());
    supplierToSave.SetAddress(txtAddress->text().toStdString());
    supplierToSave.SetSupplierType((clsSupplier::enSupplierType)cmbType->currentData().toInt());
    supplierToSave.SetBoneType(txtBoneType->text().toStdString());
    supplierToSave.SetWeeklyQuantity(weeklyQty);
    supplierToSave.SetPoints(points);

    supplierToSave.Save();

    _saved = true;
    accept();
}
