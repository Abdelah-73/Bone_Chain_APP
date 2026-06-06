#include "deliverydialog.h"

DeliveryDialog::DeliveryDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Log New Delivery");
    setFixedSize(400, 300);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    txtSupplierID = new QLineEdit(this);
    txtSupplierID->setPlaceholderText("e.g., SUP-001");

    txtProductID = new QLineEdit(this);
    txtProductID->setPlaceholderText("e.g., PROD-001 (Raw Bone)");

    txtDate = new QLineEdit(this);
    txtDate->setPlaceholderText("YYYY-MM-DD");

    txtQuantity = new QLineEdit(this);
    txtQuantity->setPlaceholderText("Quantity in KG");

    formLayout->addRow("Supplier ID:", txtSupplierID);
    formLayout->addRow("Product ID:", txtProductID);
    formLayout->addRow("Delivery Date:", txtDate);
    formLayout->addRow("Quantity (kg):", txtQuantity);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton("Submit Delivery", this);
    btnCancel = new QPushButton("Cancel", this);

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
    if (txtSupplierID->text().isEmpty() || txtQuantity->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Supplier ID and Quantity are required.");
        return;
    }

    // إنشاء ID جديد
    string newID = "DEL-" + to_string(clsDelivery::GetDeliveriesList().size() + 1);

    // إنشاء التوصيلة الجديدة (الحالة الافتراضية Pending)
    clsDelivery newDelivery = clsDelivery::GetAddNewDeliveryObject(newID);

    /* ملاحظة: لكي تعمل هذه الأكواد، يجب إضافة دوال Setters في clsDelivery
    newDelivery.SetSupplierID(txtSupplierID->text().toStdString());
    newDelivery.SetProductID(txtProductID->text().toStdString());
    newDelivery.SetDate(txtDate->text().toStdString());
    newDelivery.SetQuantity(txtQuantity->text().toDouble());
    newDelivery.Save();
    */

    _saved = true;
    accept();
}
