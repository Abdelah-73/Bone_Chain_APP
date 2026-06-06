#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "clsPerson.h"
#include "../Lib/clsString.h"

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

    static clsCustomer _ConvertLineToCustomerObject(string Line, string Separator = "#//#")
    {
        vector<string> vData = clsString::Split(Line, Separator);
        if (vData.size() == 5)
        {
            return clsCustomer(enMode::UpdateMode, vData[0], vData[1], vData[2], vData[3], (enCustomerType)stoi(vData[4]));
        }
        return GetEmptyCustomerObject();
    }

    static string _ConvertCustomerObjectToLine(const clsCustomer& Customer, string Separator = "#//#")
    {
        string Record = "";
        Record += Customer.CustomerID() + Separator;
        Record += Customer.FirstName() + Separator; // Using FirstName as Name
        Record += Customer.PhoneNumber() + Separator;
        Record += Customer.Address() + Separator;
        Record += to_string(Customer.CustomerType());
        return Record;
    }

    static vector<clsCustomer> _LoadCustomersDataFromFile(string FileName = "../Data/Customers.txt")
    {
        vector<clsCustomer> vCustomers;
        fstream MyFile;
        MyFile.open(FileName, ios::in);
        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                clsCustomer C = _ConvertLineToCustomerObject(Line);
                vCustomers.push_back(C);
            }
            MyFile.close();
        }
        return vCustomers;
    }

    static void _SaveCustomersDataToFile(const vector<clsCustomer>& vCustomers, string FileName = "../Data/Customers.txt")
    {
        fstream MyFile;
        MyFile.open(FileName, ios::out);
        if (MyFile.is_open())
        {
            for (const clsCustomer& C : vCustomers)
            {
                MyFile << _ConvertCustomerObjectToLine(C) << endl;
            }
            MyFile.close();
        }
    }

    void _AddDataLineToFile(string Line, string FileName = "../Data/Customers.txt")
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
        vector<clsCustomer> vCustomers = _LoadCustomersDataFromFile();
        for (clsCustomer& C : vCustomers)
        {
            if (C.CustomerID() == CustomerID())
            {
                C = *this;
                break;
            }
        }
        _SaveCustomersDataToFile(vCustomers);
    }

    void _AddNew()
    {
        _AddDataLineToFile(_ConvertCustomerObjectToLine(*this));
    }

public:
    clsCustomer(enMode Mode, string CustomerID, string Name, string Phone, string Address, enCustomerType CustomerType)
        : clsPerson(Name, "", "", Phone)
    {
        _Mode = Mode;
        _CustomerID = CustomerID;
        _Address = Address;
        _CustomerType = CustomerType;
    }

    bool IsEmpty() const { return _Mode == enMode::EmptyMode; }

    static clsCustomer GetEmptyCustomerObject()
    {
        return clsCustomer(enMode::EmptyMode, "", "", "", "", enCustomerType::Farmer);
    }

    string CustomerID() const { return _CustomerID; }
    void SetAddress(string Address) { _Address = Address; }
    string Address() const { return _Address; }
    void SetCustomerType(enCustomerType Type) { _CustomerType = Type; }
    enCustomerType CustomerType() const { return _CustomerType; }

    static clsCustomer Find(string CustomerID)
    {
        vector<clsCustomer> vCustomers = _LoadCustomersDataFromFile();
        for (clsCustomer& C : vCustomers)
        {
            if (C.CustomerID() == CustomerID) return C;
        }
        return GetEmptyCustomerObject();
    }

    static bool IsCustomerExist(string CustomerID)
    {
        return !Find(CustomerID).IsEmpty();
    }

    static clsCustomer GetAddNewCustomerObject(string CustomerID)
    {
        return clsCustomer(enMode::AddNewMode, CustomerID, "", "", "", enCustomerType::Farmer);
    }

    static clsCustomer CreateNewCustomer(const string& customerID, const string& name, const string& phone)
    {
        clsCustomer c(enMode::AddNewMode, customerID, name, phone, "", enCustomerType::Farmer);
        c.Save();
        return c;
    }

    bool Delete()
    {
        vector<clsCustomer> vCustomers = _LoadCustomersDataFromFile();
        for (auto it = vCustomers.begin(); it != vCustomers.end(); ++it)
        {
            if (it->CustomerID() == _CustomerID)
            {
                vCustomers.erase(it);
                _SaveCustomersDataToFile(vCustomers);
                *this = GetEmptyCustomerObject();
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

    static vector<clsCustomer> GetCustomersList()
    {
        return _LoadCustomersDataFromFile();
    }
};
