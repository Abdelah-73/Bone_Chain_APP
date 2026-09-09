#pragma once
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "../Core/clsOrder.h"

class OrderStatusDialog : public QDialog
{
    Q_OBJECT

public:
    OrderStatusDialog(int currentStatus, QWidget *parent = nullptr);
    int getSelectedStatus() const;

private:
    QComboBox *cmbStatus;
};
