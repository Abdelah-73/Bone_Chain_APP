#include "deliverydialog.h"
#include <QDateTime>
#include <QLabel>

DeliveryDialog::DeliveryDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Log New Delivery"));
    setFixedSize(420, 320);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Supplier combobox - load from real data
    cmbSupplier = new QComboBox(this);
    vector<clsSupplier> suppliers = clsSupplier::GetSuppliersList();
    for (clsSupplier &s : suppliers) {
        QString display = QString::fromStdString(s.FirstName() + " (" + s.SupplierID() + ")");
        cmbSupplier->addItem(display, QString::fromStdString(s.SupplierID()));
    }
    if (cmbSupplier->count() == 0)
        cmbSupplier->addItem(tr("-- No Suppliers Available --"), "");

    // Bone Type combobox
    cmbBoneType = new QComboBox(this);
    cmbBoneType->addItem(tr("Cow Bones"), clsDelivery::enBoneType::CowBones);
    cmbBoneType->addItem(tr("Sheep Bones"), clsDelivery::enBoneType::SheepBones);
    cmbBoneType->addItem(tr("Chicken Bones"), clsDelivery::enBoneType::ChickenBones);
    cmbBoneType->addItem(tr("Mixed"), clsDelivery::enBoneType::Mixed);

    txtDate = new QLineEdit(this);
    txtDate->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    txtDate->setPlaceholderText(tr("YYYY-MM-DD"));

    spinQuantity = new QSpinBox(this);
    spinQuantity->setRange(1, 100000);
    spinQuantity->setValue(100);
    spinQuantity->setSuffix(tr(" kg"));

    formLayout->addRow(tr("Supplier:"), cmbSupplier);
    formLayout->addRow(tr("Bone Type:"), cmbBoneType);
    formLayout->addRow(tr("Delivery Date:"), txtDate);
    formLayout->addRow(tr("Quantity:"), spinQuantity);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton(tr("Submit Delivery"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

    btnSave->setStyleSheet("background-color: #3498db; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px; border-radius: 4px;");

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, &DeliveryDialog::handleSave);
}

void DeliveryDialog::handleSave()
{
    QString supplierID = cmbSupplier->currentData().toString();
    if (supplierID.isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Please select a valid supplier."));
        return;
    }

    double qty = spinQuantity->value();
    if (qty <= 0) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Quantity must be positive."));
        return;
    }

    string newID = "DEL-" + to_string(QDateTime::currentMSecsSinceEpoch());

    clsDelivery newDelivery = clsDelivery::GetAddNewDeliveryObject(newID);
    newDelivery.SetSupplierID(supplierID.toStdString());
    newDelivery.SetProductID(cmbBoneType->currentText().toStdString());
    newDelivery.SetQuantity(qty);
    newDelivery.SetDate(txtDate->text().toStdString().empty()
        ? QDateTime::currentDateTime().toString("yyyy-MM-dd").toStdString()
        : txtDate->text().toStdString());
    newDelivery.SetStatus(clsDelivery::enStatus::Pending);
    newDelivery.Save();

    _saved = true;
    accept();
}
