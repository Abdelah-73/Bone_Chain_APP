#pragma once
#include <iostream>
#include <vector>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "clsDatabase.h"

using namespace std;

class clsOrder
{
public:
    enum enStatus { Pending = 1, Confirmed = 2, Processing = 3, Delivered = 4, Cancelled = 5 };

private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;
    string _OrderID;
    string _CustomerID;
    string _ProductID;
    int _Quantity;
    double _TotalPrice;
    string _OrderDate;
    enStatus _Status;
    bool _PointsAwarded;

    void _Update()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("UPDATE Orders SET CustomerID=?,ProductID=?,Quantity=?,TotalPrice=?,"
            "OrderDate=?,Status=?,PointsAwarded=? WHERE OrderID=?");
        q.addBindValue(QString::fromStdString(_CustomerID));
        q.addBindValue(QString::fromStdString(_ProductID));
        q.addBindValue(_Quantity);
        q.addBindValue(_TotalPrice);
        q.addBindValue(QString::fromStdString(_OrderDate));
        q.addBindValue((int)_Status);
        q.addBindValue(_PointsAwarded ? 1 : 0);
        q.addBindValue(QString::fromStdString(_OrderID));
        q.exec();
    }

    void _AddNew()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("INSERT INTO Orders VALUES (?,?,?,?,?,?,?,?)");
        q.addBindValue(QString::fromStdString(_OrderID));
        q.addBindValue(QString::fromStdString(_CustomerID));
        q.addBindValue(QString::fromStdString(_ProductID));
        q.addBindValue(_Quantity);
        q.addBindValue(_TotalPrice);
        q.addBindValue(QString::fromStdString(_OrderDate));
        q.addBindValue((int)_Status);
        q.addBindValue(_PointsAwarded ? 1 : 0);
        q.exec();
    }

public:
    clsOrder(enMode Mode, string OrderID, string CustomerID, string ProductID, int Quantity, double TotalPrice, string OrderDate, enStatus Status, bool PointsAwarded = false)
    {
        _Mode = Mode;
        _OrderID = OrderID;
        _CustomerID = CustomerID;
        _ProductID = ProductID;
        _Quantity = Quantity;
        _TotalPrice = TotalPrice;
        _OrderDate = OrderDate;
        _Status = Status;
        _PointsAwarded = PointsAwarded;
    }

    bool IsEmpty() const { return _Mode == enMode::EmptyMode; }

    static clsOrder GetEmptyOrderObject()
    {
        return clsOrder(enMode::EmptyMode, "", "", "", 0, 0.0, "", enStatus::Pending, false);
    }

    string OrderID() const { return _OrderID; }
    void SetCustomerID(string CustomerID) { _CustomerID = CustomerID; }
    string CustomerID() const { return _CustomerID; }
    void SetProductID(string ProductID) { _ProductID = ProductID; }
    string ProductID() const { return _ProductID; }
    void SetQuantity(int Quantity) { _Quantity = Quantity; }
    int Quantity() const { return _Quantity; }
    void SetTotalPrice(double TotalPrice) { _TotalPrice = TotalPrice; }
    double TotalPrice() const { return _TotalPrice; }
    void SetOrderDate(string OrderDate) { _OrderDate = OrderDate; }
    string OrderDate() const { return _OrderDate; }
    void SetStatus(enStatus Status) { _Status = Status; }
    enStatus Status() const { return _Status; }
    void SetPointsAwarded(bool Awarded) { _PointsAwarded = Awarded; }
    bool PointsAwarded() const { return _PointsAwarded; }

    static clsOrder Find(string OrderID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT * FROM Orders WHERE OrderID=?");
        q.addBindValue(QString::fromStdString(OrderID));
        if (q.exec() && q.next())
        {
            return clsOrder(enMode::UpdateMode,
                q.value(0).toString().toStdString(),
                q.value(1).toString().toStdString(),
                q.value(2).toString().toStdString(),
                q.value(3).toInt(),
                q.value(4).toDouble(),
                q.value(5).toString().toStdString(),
                (enStatus)q.value(6).toInt(),
                q.value(7).toInt() != 0);
        }
        return GetEmptyOrderObject();
    }

    static bool IsOrderExist(string OrderID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT COUNT(*) FROM Orders WHERE OrderID=?");
        q.addBindValue(QString::fromStdString(OrderID));
        if (q.exec() && q.next())
            return q.value(0).toInt() > 0;
        return false;
    }

    static clsOrder GetAddNewOrderObject(string OrderID)
    {
        return clsOrder(enMode::AddNewMode, OrderID, "", "", 0, 0.0, "", enStatus::Pending, false);
    }

    bool Cancel()
    {
        if (_Status == enStatus::Pending || _Status == enStatus::Confirmed)
        {
            _Status = enStatus::Cancelled;
            Save();
            return true;
        }
        return false;
    }

    void Save()
    {
        switch (_Mode)
        {
        case enMode::EmptyMode:
            break;
        case enMode::UpdateMode:
            _Update();
            break;
        case enMode::AddNewMode:
            _AddNew();
            _Mode = enMode::UpdateMode;
            break;
        }
    }

    static vector<clsOrder> GetOrdersList()
    {
        vector<clsOrder> vOrders;
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        if (q.exec("SELECT * FROM Orders"))
        {
            while (q.next())
            {
                vOrders.push_back(clsOrder(enMode::UpdateMode,
                    q.value(0).toString().toStdString(),
                    q.value(1).toString().toStdString(),
                    q.value(2).toString().toStdString(),
                    q.value(3).toInt(),
                    q.value(4).toDouble(),
                    q.value(5).toString().toStdString(),
                    (enStatus)q.value(6).toInt(),
                    q.value(7).toInt() != 0));
            }
        }
        return vOrders;
    }
};
