#include "orderstatusdialog.h"

OrderStatusDialog::OrderStatusDialog(int currentStatus, QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Update Order Status");
    setFixedSize(250, 150);
    setStyleSheet("background-color: #ffffff; color: #2c3e50; font-size: 14px;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *lblInfo = new QLabel("Select the new status:", this);
    lblInfo->setStyleSheet("font-weight: bold; margin-bottom: 5px;");

    cmbStatus = new QComboBox(this);
    cmbStatus->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px; font-weight: bold;");

    // Add items with their underlying Data Value, then set their specific Colors!

    // 1. Pending (Orange)
    cmbStatus->addItem("Pending", clsOrder::enStatus::Pending);
    cmbStatus->setItemData(0, QBrush(QColor("#f39c12")), Qt::ForegroundRole);

    // 2. Confirmed (Blue)
    cmbStatus->addItem("Confirmed", clsOrder::enStatus::Confirmed);
    cmbStatus->setItemData(1, QBrush(QColor("#3498db")), Qt::ForegroundRole);

    // 3. Processing (Purple)
    cmbStatus->addItem("Processing", clsOrder::enStatus::Processing);
    cmbStatus->setItemData(2, QBrush(QColor("#9b59b6")), Qt::ForegroundRole);

    // 4. Delivered (Green)
    cmbStatus->addItem("Delivered", clsOrder::enStatus::Delivered);
    cmbStatus->setItemData(3, QBrush(QColor("#27ae60")), Qt::ForegroundRole);

    // 5. Cancelled (Red)
    cmbStatus->addItem("Cancelled", clsOrder::enStatus::Cancelled);
    cmbStatus->setItemData(4, QBrush(QColor("#e74c3c")), Qt::ForegroundRole);

    // Automatically select the order's current status when the window opens
    int index = cmbStatus->findData(currentStatus);
    if (index != -1) {
        cmbStatus->setCurrentIndex(index);
    }

    QPushButton *btnSave = new QPushButton("Save Status", this);
    btnSave->setStyleSheet("background-color: #2980b9; color: white; padding: 8px; border-radius: 4px; font-weight: bold; margin-top: 10px;");

    layout->addWidget(lblInfo);
    layout->addWidget(cmbStatus);
    layout->addStretch();
    layout->addWidget(btnSave);

    connect(btnSave, &QPushButton::clicked, this, &QDialog::accept);
}

int OrderStatusDialog::getSelectedStatus() const
{
    // Return the integer value (1, 2, 3, 4, or 5)
    return cmbStatus->currentData().toInt();
}
