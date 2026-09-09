#pragma once
#include <iostream>
#include <vector>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "clsPerson.h"
#include "clsDatabase.h"

using namespace std;

class clsSupplier : public clsPerson
{
public:
    enum enSupplierType { Butcher = 1, Restaurant = 2, Individual = 3 };

private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;
    string _SupplierID;
    string _Address;
    enSupplierType _SupplierType;
    string _BoneType;
    double _WeeklyQuantity;
    int _Points;

    void _Update()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("UPDATE Suppliers SET Name=?,Phone=?,Address=?,SupplierType=?,"
            "BoneType=?,WeeklyQuantity=?,Points=? WHERE SupplierID=?");
        q.addBindValue(QString::fromStdString(FirstName()));
        q.addBindValue(QString::fromStdString(PhoneNumber()));
        q.addBindValue(QString::fromStdString(_Address));
        q.addBindValue((int)_SupplierType);
        q.addBindValue(QString::fromStdString(_BoneType));
        q.addBindValue(_WeeklyQuantity);
        q.addBindValue(_Points);
        q.addBindValue(QString::fromStdString(_SupplierID));
        q.exec();
    }

    void _AddNew()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("INSERT INTO Suppliers VALUES (?,?,?,?,?,?,?,?)");
        q.addBindValue(QString::fromStdString(_SupplierID));
        q.addBindValue(QString::fromStdString(FirstName()));
        q.addBindValue(QString::fromStdString(PhoneNumber()));
        q.addBindValue(QString::fromStdString(_Address));
        q.addBindValue((int)_SupplierType);
        q.addBindValue(QString::fromStdString(_BoneType));
        q.addBindValue(_WeeklyQuantity);
        q.addBindValue(_Points);
        q.exec();
    }

public:
    clsSupplier(enMode Mode, string SupplierID, string Name, string Phone, string Address, enSupplierType SupplierType, string BoneType, double WeeklyQuantity, int Points)
        : clsPerson(Name, "", "", Phone)
    {
        _Mode = Mode;
        _SupplierID = SupplierID;
        _Address = Address;
        _SupplierType = SupplierType;
        _BoneType = BoneType;
        _WeeklyQuantity = WeeklyQuantity;
        _Points = Points;
    }

    bool IsEmpty() const { return _Mode == enMode::EmptyMode; }

    static clsSupplier GetEmptySupplierObject()
    {
        return clsSupplier(enMode::EmptyMode, "", "", "", "", enSupplierType::Individual, "", 0.0, 0);
    }

    string SupplierID() const { return _SupplierID; }
    void SetAddress(string Address) { _Address = Address; }
    string Address() const { return _Address; }
    void SetSupplierType(enSupplierType Type) { _SupplierType = Type; }
    enSupplierType SupplierType() const { return _SupplierType; }
    void SetBoneType(string BoneType) { _BoneType = BoneType; }
    string BoneType() const { return _BoneType; }
    void SetWeeklyQuantity(double WeeklyQuantity) { _WeeklyQuantity = WeeklyQuantity; }
    double WeeklyQuantity() const { return _WeeklyQuantity; }
    void SetPoints(int Points) { _Points = Points; }
    int Points() const { return _Points; }

    static clsSupplier Find(string SupplierID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT * FROM Suppliers WHERE SupplierID=?");
        q.addBindValue(QString::fromStdString(SupplierID));
        if (q.exec() && q.next())
        {
            return clsSupplier(enMode::UpdateMode,
                q.value(0).toString().toStdString(),
                q.value(1).toString().toStdString(),
                q.value(2).toString().toStdString(),
                q.value(3).toString().toStdString(),
                (enSupplierType)q.value(4).toInt(),
                q.value(5).toString().toStdString(),
                q.value(6).toDouble(),
                q.value(7).toInt());
        }
        return GetEmptySupplierObject();
    }

    static bool IsSupplierExist(string SupplierID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT COUNT(*) FROM Suppliers WHERE SupplierID=?");
        q.addBindValue(QString::fromStdString(SupplierID));
        if (q.exec() && q.next())
            return q.value(0).toInt() > 0;
        return false;
    }

    static clsSupplier GetAddNewSupplierObject(string SupplierID)
    {
        return clsSupplier(enMode::AddNewMode, SupplierID, "", "", "", enSupplierType::Individual, "", 0.0, 0);
    }

    static clsSupplier CreateNewSupplier(const string& supplierID, const string& name, const string& phone)
    {
        clsSupplier s(enMode::AddNewMode, supplierID, name, phone, "", enSupplierType::Individual, "", 0.0, 0);
        s.Save();
        return s;
    }

    bool Delete()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("DELETE FROM Suppliers WHERE SupplierID=?");
        q.addBindValue(QString::fromStdString(_SupplierID));
        if (q.exec() && q.numRowsAffected() > 0)
        {
            *this = GetEmptySupplierObject();
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

    void RecordDelivery(double Quantity)
    {
        _WeeklyQuantity += Quantity;
        _Points += (int)(Quantity * 10);
        Save();
    }

    static vector<clsSupplier> GetSuppliersList()
    {
        vector<clsSupplier> vSuppliers;
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        if (q.exec("SELECT * FROM Suppliers"))
        {
            while (q.next())
            {
                vSuppliers.push_back(clsSupplier(enMode::UpdateMode,
                    q.value(0).toString().toStdString(),
                    q.value(1).toString().toStdString(),
                    q.value(2).toString().toStdString(),
                    q.value(3).toString().toStdString(),
                    (enSupplierType)q.value(4).toInt(),
                    q.value(5).toString().toStdString(),
                    q.value(6).toDouble(),
                    q.value(7).toInt()));
            }
        }
        return vSuppliers;
    }
};
