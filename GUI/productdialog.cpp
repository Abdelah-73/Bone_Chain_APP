#include "productdialog.h"

ProductDialog::ProductDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Product Catalog - Add New"));
    setFixedSize(400, 450);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Initialize Fields
    txtName = new QLineEdit(this);

    // Initialize Categories
    cmbCategory = new QComboBox(this);
    cmbCategory->addItem(tr("Raw Bone Fertilizer"), clsProduct::enCategory::RawBoneFertilizer);
    cmbCategory->addItem(tr("Powder Fertilizer"), clsProduct::enCategory::PowderFertilizer);
    cmbCategory->addItem(tr("Organic Fertilizer"), clsProduct::enCategory::OrganicFertilizer);
    cmbCategory->addItem(tr("Feed Supplement"), clsProduct::enCategory::FeedSupplement);
    cmbCategory->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");

    txtPrice = new QLineEdit(this);
    txtPrice->setPlaceholderText(tr("0.00"));

    txtStockQuantity = new QLineEdit(this);
    txtStockQuantity->setPlaceholderText(tr("0"));

    txtMinimumStock = new QLineEdit(this);
    txtMinimumStock->setPlaceholderText(tr("20"));

    txtProductionDate = new QLineEdit(this);
    txtProductionDate->setPlaceholderText(tr("YYYY-MM-DD"));

    txtExpiryDate = new QLineEdit(this);
    txtExpiryDate->setPlaceholderText(tr("YYYY-MM-DD"));

    // Add to form
    formLayout->addRow(tr("Product Name:"), txtName);
    formLayout->addRow(tr("Category:"), cmbCategory);
    formLayout->addRow(tr("Price ($):"), txtPrice);
    formLayout->addRow(tr("Current Stock:"), txtStockQuantity);
    formLayout->addRow(tr("Minimum Stock Alert:"), txtMinimumStock);
    formLayout->addRow(tr("Production Date:"), txtProductionDate);
    formLayout->addRow(tr("Expiry Date:"), txtExpiryDate);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnSave = new QPushButton(tr("Save Product"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

    btnSave->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #95a5a6; color: white; padding: 8px; border-radius: 4px;");

    buttonLayout->addStretch();
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, this, &ProductDialog::handleSave);
}

void ProductDialog::loadProductForEdit(const string& productID)
{
    _currentProductID = productID;
    clsProduct product = clsProduct::Find(productID);

    if (!product.IsEmpty())
    {
        setWindowTitle(tr("Product Catalog - Edit"));
        btnSave->setText(tr("Update Product"));

        // Pre-fill existing data
        txtName->setText(QString::fromStdString(product.Name()));

        int index = cmbCategory->findData(product.Category());
        if (index != -1) cmbCategory->setCurrentIndex(index);

        txtPrice->setText(QString::number(product.Price(), 'f', 2));
        txtStockQuantity->setText(QString::number(product.StockQuantity()));
        txtMinimumStock->setText(QString::number(product.MinimumStock()));
        txtProductionDate->setText(QString::fromStdString(product.ProductionDate()));
        txtExpiryDate->setText(QString::fromStdString(product.ExpiryDate()));
    }
}

void ProductDialog::handleSave()
{
    if (txtName->text().isEmpty() || txtPrice->text().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Product Name and Price are required."));
        return;
    }

    clsProduct productToSave = clsProduct::GetEmptyProductObject();

    if (_currentProductID.empty()) {
        // Add Mode
        int maxNum = 0;
        for (const auto& p : clsProduct::GetProductsList()) {
            string pid = p.ProductID();
            if (pid.size() > 5 && pid.substr(0, 5) == "PROD-") {
                try { int n = stoi(pid.substr(5)); if (n > maxNum) maxNum = n; }
                catch (...) {}
            }
        }
        string newID = "PROD-" + to_string(maxNum + 1);
        productToSave = clsProduct::GetAddNewProductObject(newID);
    } else {
        // Update Mode
        productToSave = clsProduct::Find(_currentProductID);
    }

    bool ok;
    double price = txtPrice->text().toDouble(&ok);
    if (!ok || price < 0) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Price must be a valid positive number."));
        return;
    }

    int stock = txtStockQuantity->text().toInt(&ok);
    if (!ok || stock < 0) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Stock must be a valid non-negative integer."));
        return;
    }

    int minStock = txtMinimumStock->text().toInt(&ok);
    if (!ok || minStock < 0) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Minimum Stock must be a valid non-negative integer."));
        return;
    }

    productToSave.SetName(txtName->text().toStdString());
    productToSave.SetCategory((clsProduct::enCategory)cmbCategory->currentData().toInt());
    productToSave.SetPrice(price);
    productToSave.SetStockQuantity(stock);
    productToSave.SetMinimumStock(minStock);
    productToSave.SetProductionDate(txtProductionDate->text().toStdString());
    productToSave.SetExpiryDate(txtExpiryDate->text().toStdString());

    productToSave.Save();

    _saved = true;
    accept();
}
