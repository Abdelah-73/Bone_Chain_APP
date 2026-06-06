#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../Lib/clsString.h" // Ensures you can use your Split function

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

    // ========================================================
    // PRIVATE HELPER METHODS (File I/O Translation)
    // ========================================================

    static clsDelivery _ConvertLineToDeliveryObject(string Line, string Separator = "#//#")
    {
        vector<string> vDeliveryData = clsString::Split(Line, Separator);
        if (vDeliveryData.size() != 6)
            return GetEmptyDeliveryObject();

        return clsDelivery(enMode::UpdateMode,
                           vDeliveryData[0],
                           vDeliveryData[1],
                           vDeliveryData[2],
                           vDeliveryData[3],
                           stod(vDeliveryData[4]),
                           (enStatus)stoi(vDeliveryData[5]));
    }

    static string _ConvertDeliveryObjectToLine(const clsDelivery& Delivery, string Separator = "#//#")
    {
        string stDeliveryRecord = "";
        stDeliveryRecord += Delivery.DeliveryID() + Separator;
        stDeliveryRecord += Delivery.SupplierID() + Separator;
        stDeliveryRecord += Delivery.ProductID() + Separator;
        stDeliveryRecord += Delivery.Date() + Separator;
        stDeliveryRecord += to_string(Delivery.Quantity()) + Separator;
        stDeliveryRecord += to_string((int)Delivery.Status());

        return stDeliveryRecord;
    }

    static void _SaveDeliveriesDataToFile(const vector<clsDelivery>& vDeliveries)
    {
        fstream MyFile;
        MyFile.open("../Data/Deliveries.txt", ios::out);

        if (MyFile.is_open())
        {
            for (const clsDelivery& C : vDeliveries)
            {
                MyFile << _ConvertDeliveryObjectToLine(C) << endl;
            }
            MyFile.close();
        }
    }

public:
    // ========================================================
    // CONSTRUCTOR & GETTERS/SETTERS
    // ========================================================

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

    // BoneType helpers (ProductID stores the bone type string)
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

    // ========================================================
    // STATIC FACTORY METHODS
    // ========================================================

    static clsDelivery GetEmptyDeliveryObject()
    {
        return clsDelivery(enMode::EmptyMode, "", "", "", "", 0, enStatus::Pending);
    }

    static clsDelivery GetAddNewDeliveryObject(string DeliveryID)
    {
        return clsDelivery(enMode::AddNewMode, DeliveryID, "", "", "", 0, enStatus::Pending);
    }

    // ========================================================
    // CORE FILE I/O METHODS (Find, Read All, Save)
    // ========================================================

    static vector<clsDelivery> GetDeliveriesList()
    {
        vector<clsDelivery> vDeliveries;
        fstream MyFile;
        MyFile.open("../Data/Deliveries.txt", ios::in); // ios::in is Read Mode

        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                if (Line != "")
                {
                    clsDelivery Delivery = _ConvertLineToDeliveryObject(Line);
                    vDeliveries.push_back(Delivery);
                }
            }
            MyFile.close();
        }
        return vDeliveries;
    }

    static clsDelivery Find(string DeliveryID)
    {
        vector<clsDelivery> vDeliveries = GetDeliveriesList();

        for (clsDelivery Delivery : vDeliveries)
        {
            if (Delivery.DeliveryID() == DeliveryID)
            {
                return Delivery;
            }
        }

        return GetEmptyDeliveryObject();
    }

    void Save()
    {
        vector<clsDelivery> vDeliveries = GetDeliveriesList();

        switch (_Mode)
        {
            case enMode::EmptyMode:
            {
                return;
            }
            case enMode::UpdateMode:
            {
                for (clsDelivery& D : vDeliveries)
                {
                    if (D.DeliveryID() == DeliveryID())
                    {
                        D = *this;
                        break;
                    }
                }
                _SaveDeliveriesDataToFile(vDeliveries);
                break;
            }
            case enMode::AddNewMode:
            {
                vDeliveries.push_back(*this);
                _SaveDeliveriesDataToFile(vDeliveries);
                _Mode = enMode::UpdateMode;
                break;
            }
        }
    }
};
