#pragma once
#include <iostream>
#include <vector>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "clsPerson.h"
#include "clsDatabase.h"

using namespace std;

class clsCustomer : public clsPerson
{
public:
    enum enCustomerType { Farmer = 1, Breeder = 2, Company = 3 };

private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;
    string _CustomerID;
    string _Address;
    enCustomerType _CustomerType;
    int _Points;

    void _Update()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("UPDATE Customers SET Name=?,Phone=?,Address=?,CustomerType=?,Points=? WHERE CustomerID=?");
        q.addBindValue(QString::fromStdString(FirstName()));
        q.addBindValue(QString::fromStdString(PhoneNumber()));
        q.addBindValue(QString::fromStdString(_Address));
        q.addBindValue((int)_CustomerType);
        q.addBindValue(_Points);
        q.addBindValue(QString::fromStdString(_CustomerID));
        q.exec();
    }

    void _AddNew()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("INSERT INTO Customers VALUES (?,?,?,?,?,?)");
        q.addBindValue(QString::fromStdString(_CustomerID));
        q.addBindValue(QString::fromStdString(FirstName()));
        q.addBindValue(QString::fromStdString(PhoneNumber()));
        q.addBindValue(QString::fromStdString(_Address));
        q.addBindValue((int)_CustomerType);
        q.addBindValue(_Points);
        q.exec();
    }

public:
    clsCustomer(enMode Mode, string CustomerID, string Name, string Phone, string Address, enCustomerType CustomerType, int Points = 0)
        : clsPerson(Name, "", "", Phone)
    {
        _Mode = Mode;
        _CustomerID = CustomerID;
        _Address = Address;
        _CustomerType = CustomerType;
        _Points = Points;
    }

    bool IsEmpty() const { return _Mode == enMode::EmptyMode; }

    static clsCustomer GetEmptyCustomerObject()
    {
        return clsCustomer(enMode::EmptyMode, "", "", "", "", enCustomerType::Farmer, 0);
    }

    string CustomerID() const { return _CustomerID; }
    void SetAddress(string Address) { _Address = Address; }
    string Address() const { return _Address; }
    void SetCustomerType(enCustomerType Type) { _CustomerType = Type; }
    enCustomerType CustomerType() const { return _CustomerType; }
    void SetPoints(int Points) { _Points = Points; }
    int Points() const { return _Points; }

    static clsCustomer Find(string CustomerID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT * FROM Customers WHERE CustomerID=?");
        q.addBindValue(QString::fromStdString(CustomerID));
        if (q.exec() && q.next())
        {
            return clsCustomer(enMode::UpdateMode,
                q.value(0).toString().toStdString(),
                q.value(1).toString().toStdString(),
                q.value(2).toString().toStdString(),
                q.value(3).toString().toStdString(),
                (enCustomerType)q.value(4).toInt(),
                q.value(5).toInt());
        }
        return GetEmptyCustomerObject();
    }

    static bool IsCustomerExist(string CustomerID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT COUNT(*) FROM Customers WHERE CustomerID=?");
        q.addBindValue(QString::fromStdString(CustomerID));
        if (q.exec() && q.next())
            return q.value(0).toInt() > 0;
        return false;
    }

    static clsCustomer GetAddNewCustomerObject(string CustomerID)
    {
        return clsCustomer(enMode::AddNewMode, CustomerID, "", "", "", enCustomerType::Farmer, 0);
    }

    static clsCustomer CreateNewCustomer(const string& customerID, const string& name, const string& phone)
    {
        clsCustomer c(enMode::AddNewMode, customerID, name, phone, "", enCustomerType::Farmer, 0);
        c.Save();
        return c;
    }

    bool Delete()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("DELETE FROM Customers WHERE CustomerID=?");
        q.addBindValue(QString::fromStdString(_CustomerID));
        if (q.exec() && q.numRowsAffected() > 0)
        {
            *this = GetEmptyCustomerObject();
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

    static vector<clsCustomer> GetCustomersList()
    {
        vector<clsCustomer> vCustomers;
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        if (q.exec("SELECT * FROM Customers"))
        {
            while (q.next())
            {
                vCustomers.push_back(clsCustomer(enMode::UpdateMode,
                    q.value(0).toString().toStdString(),
                    q.value(1).toString().toStdString(),
                    q.value(2).toString().toStdString(),
                    q.value(3).toString().toStdString(),
                    (enCustomerType)q.value(4).toInt(),
                    q.value(5).toInt()));
            }
        }
        return vCustomers;
    }
};
