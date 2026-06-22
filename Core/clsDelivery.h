#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "clsDatabase.h"

using namespace std;

class clsDelivery
{
public:
    enum enStatus { Pending = 1, Approved = 2, Rejected = 3 };
    enum enBoneType { CowBones = 1, SheepBones = 2, ChickenBones = 3, Mixed = 4 };

private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;

    string _DeliveryID;
    string _SupplierID;
    string _ProductID;
    string _Date;
    double _Quantity;
    enStatus _Status;

public:
    clsDelivery(enMode Mode, string DeliveryID, string SupplierID, string ProductID, string Date, double Quantity, enStatus Status)
    {
        _Mode = Mode;
        _DeliveryID = DeliveryID;
        _SupplierID = SupplierID;
        _ProductID = ProductID;
        _Date = Date;
        _Quantity = Quantity;
        _Status = Status;
    }

    string DeliveryID() const { return _DeliveryID; }
    string SupplierID() const { return _SupplierID; }
    string ProductID() const { return _ProductID; }
    string Date() const { return _Date; }
    double Quantity() const { return _Quantity; }
    enStatus Status() const { return _Status; }

    void SetStatus(enStatus Status) { _Status = Status; }
    void SetQuantity(double Quantity) { _Quantity = Quantity; }
    void SetDate(string Date) { _Date = Date; }
    void SetSupplierID(string SupplierID) { _SupplierID = SupplierID; }
    void SetProductID(string ProductID) { _ProductID = ProductID; }
    bool IsEmpty() const { return _Mode == enMode::EmptyMode; }

    enBoneType BoneTypeEnum()
    {
        if (_ProductID == "Cow Bones") return enBoneType::CowBones;
        if (_ProductID == "Sheep Bones") return enBoneType::SheepBones;
        if (_ProductID == "Chicken Bones") return enBoneType::ChickenBones;
        if (_ProductID == "Mixed") return enBoneType::Mixed;
        return enBoneType::CowBones;
    }

    static string BoneTypeToString(enBoneType bt)
    {
        switch (bt)
        {
            case enBoneType::CowBones: return "Cow Bones";
            case enBoneType::SheepBones: return "Sheep Bones";
            case enBoneType::ChickenBones: return "Chicken Bones";
            case enBoneType::Mixed: return "Mixed";
            default: return "Cow Bones";
        }
    }

    static clsDelivery GetEmptyDeliveryObject()
    {
        return clsDelivery(enMode::EmptyMode, "", "", "", "", 0, enStatus::Pending);
    }

    static clsDelivery GetAddNewDeliveryObject(string DeliveryID)
    {
        return clsDelivery(enMode::AddNewMode, DeliveryID, "", "", "", 0, enStatus::Pending);
    }

    static vector<clsDelivery> GetDeliveriesList()
    {
        vector<clsDelivery> vDeliveries;
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        if (q.exec("SELECT * FROM Deliveries"))
        {
            while (q.next())
            {
                vDeliveries.push_back(clsDelivery(enMode::UpdateMode,
                    q.value(0).toString().toStdString(),
                    q.value(1).toString().toStdString(),
                    q.value(2).toString().toStdString(),
                    q.value(3).toString().toStdString(),
                    q.value(4).toDouble(),
                    (enStatus)q.value(5).toInt()));
            }
        }
        return vDeliveries;
    }

    static clsDelivery Find(string DeliveryID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT * FROM Deliveries WHERE DeliveryID=?");
        q.addBindValue(QString::fromStdString(DeliveryID));
        if (q.exec() && q.next())
        {
            return clsDelivery(enMode::UpdateMode,
                q.value(0).toString().toStdString(),
                q.value(1).toString().toStdString(),
                q.value(2).toString().toStdString(),
                q.value(3).toString().toStdString(),
                q.value(4).toDouble(),
                (enStatus)q.value(5).toInt());
        }
        return GetEmptyDeliveryObject();
    }

    void Save()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());

        switch (_Mode)
        {
            case enMode::EmptyMode:
                return;

            case enMode::UpdateMode:
            {
                q.prepare("UPDATE Deliveries SET SupplierID=?,ProductID=?,Date=?,Quantity=?,Status=? WHERE DeliveryID=?");
                q.addBindValue(QString::fromStdString(_SupplierID));
                q.addBindValue(QString::fromStdString(_ProductID));
                q.addBindValue(QString::fromStdString(_Date));
                q.addBindValue(_Quantity);
                q.addBindValue((int)_Status);
                q.addBindValue(QString::fromStdString(_DeliveryID));
                q.exec();
                break;
            }

            case enMode::AddNewMode:
            {
                q.prepare("INSERT INTO Deliveries VALUES (?,?,?,?,?,?)");
                q.addBindValue(QString::fromStdString(_DeliveryID));
                q.addBindValue(QString::fromStdString(_SupplierID));
                q.addBindValue(QString::fromStdString(_ProductID));
                q.addBindValue(QString::fromStdString(_Date));
                q.addBindValue(_Quantity);
                q.addBindValue((int)_Status);
                q.exec();
                _Mode = enMode::UpdateMode;
                break;
            }
        }
    }
};
