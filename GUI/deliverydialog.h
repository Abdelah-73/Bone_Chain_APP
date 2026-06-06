#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <string>
#include "../Core/clsDelivery.h"

using namespace std;

class DeliveryDialog : public QDialog
{
    Q_OBJECT

public:
    DeliveryDialog(QWidget *parent = nullptr);
    bool wasSaved() const { return _saved; }

private slots:
    void handleSave();

private:
    QLineEdit *txtSupplierID;
    QLineEdit *txtProductID; // نوع العظم
    QLineEdit *txtDate;
    QLineEdit *txtQuantity;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool _saved = false;
};
