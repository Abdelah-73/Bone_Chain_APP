#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "clsPerson.h"
#include "../Lib/clsString.h"

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

    static clsSupplier _ConvertLineToSupplierObject(string Line, string Separator = "#//#")
    {
        vector<string> vData = clsString::Split(Line, Separator);
        if (vData.size() == 8)
        {
            return clsSupplier(enMode::UpdateMode, vData[0], vData[1], vData[2], vData[3], (enSupplierType)stoi(vData[4]), vData[5], stod(vData[6]), stoi(vData[7]));
        }
        return GetEmptySupplierObject();
    }

    static string _ConvertSupplierObjectToLine(const clsSupplier& Supplier, string Separator = "#//#")
    {
        string Record = "";
        Record += Supplier.SupplierID() + Separator;
        Record += Supplier.FirstName() + Separator;
        Record += Supplier.PhoneNumber() + Separator;
        Record += Supplier.Address() + Separator;
        Record += to_string(Supplier.SupplierType()) + Separator;
        Record += Supplier.BoneType() + Separator;
        Record += to_string(Supplier.WeeklyQuantity()) + Separator;
        Record += to_string(Supplier.Points());
        return Record;
    }

    static vector<clsSupplier> _LoadSuppliersDataFromFile(string FileName = "../Data/Suppliers.txt")
    {
        vector<clsSupplier> vSuppliers;
        fstream MyFile;
        MyFile.open(FileName, ios::in);
        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                clsSupplier S = _ConvertLineToSupplierObject(Line);
                vSuppliers.push_back(S);
            }
            MyFile.close();
        }
        return vSuppliers;
    }

    static void _SaveSuppliersDataToFile(const vector<clsSupplier>& vSuppliers, string FileName = "../Data/Suppliers.txt")
    {
        fstream MyFile;
        MyFile.open(FileName, ios::out);
        if (MyFile.is_open())
        {
            for (const clsSupplier& S : vSuppliers)
            {
                MyFile << _ConvertSupplierObjectToLine(S) << endl;
            }
            MyFile.close();
        }
    }

    void _AddDataLineToFile(string Line, string FileName = "../Data/Suppliers.txt")
    {
        fstream MyFile;
        MyFile.open(FileName, ios::out | ios::app);
        if (MyFile.is_open())
        {
            MyFile << Line << endl;
            MyFile.close();
        }
    }

    void _Update()
    {
        vector<clsSupplier> vSuppliers = _LoadSuppliersDataFromFile();
        for (clsSupplier& S : vSuppliers)
        {
            if (S.SupplierID() == SupplierID())
            {
                S = *this;
                break;
            }
        }
        _SaveSuppliersDataToFile(vSuppliers);
    }

    void _AddNew()
    {
        _AddDataLineToFile(_ConvertSupplierObjectToLine(*this));
    }

public:
    clsSupplier(enMode Mode, string SupplierID, string Name, string Phone, string Address, enSupplierType SupplierType, string BoneType, double WeeklyQuantity, int Points)
        : clsPerson(Name, "", "", Phone) // Passing Name to FirstName, empty LastName and Email
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
        vector<clsSupplier> vSuppliers = _LoadSuppliersDataFromFile();
        for (clsSupplier& S : vSuppliers)
        {
            if (S.SupplierID() == SupplierID) return S;
        }
        return GetEmptySupplierObject();
    }

    static bool IsSupplierExist(string SupplierID)
    {
        return !Find(SupplierID).IsEmpty();
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
        vector<clsSupplier> vSuppliers = _LoadSuppliersDataFromFile();
        for (auto it = vSuppliers.begin(); it != vSuppliers.end(); ++it)
        {
            if (it->SupplierID() == _SupplierID)
            {
                vSuppliers.erase(it);
                _SaveSuppliersDataToFile(vSuppliers);
                *this = GetEmptySupplierObject();
                return true;
            }
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
        _Points += (int)(Quantity * 10); // Simple logic: 10 points per kg delivered
        Save();
    }

    static vector<clsSupplier> GetSuppliersList()
    {
        return _LoadSuppliersDataFromFile();
    }
};
