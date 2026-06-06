#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <string>
#include "../Core/clsDelivery.h"
#include "../Core/clsSupplier.h"

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
    QComboBox *cmbSupplier;
    QComboBox *cmbBoneType;
    QLineEdit *txtDate;
    QSpinBox *spinQuantity;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    bool _saved = false;
};
