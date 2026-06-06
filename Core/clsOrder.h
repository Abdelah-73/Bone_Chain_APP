#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "../Lib/clsString.h"

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

    static clsOrder _ConvertLineToOrderObject(string Line, string Separator = "#//#")
    {
        vector<string> vData = clsString::Split(Line, Separator);
        if (vData.size() == 7)
        {
            return clsOrder(enMode::UpdateMode, vData[0], vData[1], vData[2], stoi(vData[3]), stod(vData[4]), vData[5], (enStatus)stoi(vData[6]));
        }
        return GetEmptyOrderObject();
    }

    static string _ConvertOrderObjectToLine(clsOrder Order, string Separator = "#//#")
    {
        string Record = "";
        Record += Order.OrderID() + Separator;
        Record += Order.CustomerID() + Separator;
        Record += Order.ProductID() + Separator;
        Record += to_string(Order.Quantity()) + Separator;
        Record += to_string(Order.TotalPrice()) + Separator;
        Record += Order.OrderDate() + Separator;
        Record += to_string(Order.Status());
        return Record;
    }

    static vector<clsOrder> _LoadOrdersDataFromFile(string FileName = "../Data/Orders.txt")
    {
        vector<clsOrder> vOrders;
        fstream MyFile;
        MyFile.open(FileName, ios::in);
        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                clsOrder O = _ConvertLineToOrderObject(Line);
                vOrders.push_back(O);
            }
            MyFile.close();
        }
        return vOrders;
    }

    static void _SaveOrdersDataToFile(vector<clsOrder>& vOrders, string FileName = "../Data/Orders.txt")
    {
        fstream MyFile;
        MyFile.open(FileName, ios::out);
        if (MyFile.is_open())
        {
            for (clsOrder& O : vOrders)
            {
                MyFile << _ConvertOrderObjectToLine(O) << endl;
            }
            MyFile.close();
        }
    }

    void _AddDataLineToFile(string Line, string FileName = "../Data/Orders.txt")
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
        vector<clsOrder> vOrders = _LoadOrdersDataFromFile();
        for (clsOrder& O : vOrders)
        {
            if (O.OrderID() == OrderID())
            {
                O = *this;
                break;
            }
        }
        _SaveOrdersDataToFile(vOrders);
    }

    void _AddNew()
    {
        _AddDataLineToFile(_ConvertOrderObjectToLine(*this));
    }

public:
    clsOrder(enMode Mode, string OrderID, string CustomerID, string ProductID, int Quantity, double TotalPrice, string OrderDate, enStatus Status)
    {
        _Mode = Mode;
        _OrderID = OrderID;
        _CustomerID = CustomerID;
        _ProductID = ProductID;
        _Quantity = Quantity;
        _TotalPrice = TotalPrice;
        _OrderDate = OrderDate;
        _Status = Status;
    }

    bool IsEmpty() { return _Mode == enMode::EmptyMode; }

    static clsOrder GetEmptyOrderObject()
    {
        return clsOrder(enMode::EmptyMode, "", "", "", 0, 0.0, "", enStatus::Pending);
    }

    string OrderID() { return _OrderID; }
    void SetCustomerID(string CustomerID) { _CustomerID = CustomerID; }
    string CustomerID() { return _CustomerID; }
    void SetProductID(string ProductID) { _ProductID = ProductID; }
    string ProductID() { return _ProductID; }
    void SetQuantity(int Quantity) { _Quantity = Quantity; }
    int Quantity() { return _Quantity; }
    void SetTotalPrice(double TotalPrice) { _TotalPrice = TotalPrice; }
    double TotalPrice() { return _TotalPrice; }
    void SetOrderDate(string OrderDate) { _OrderDate = OrderDate; }
    string OrderDate() { return _OrderDate; }
    void SetStatus(enStatus Status) { _Status = Status; }
    enStatus Status() { return _Status; }

    static clsOrder Find(string OrderID)
    {
        vector<clsOrder> vOrders = _LoadOrdersDataFromFile();
        for (clsOrder& O : vOrders)
        {
            if (O.OrderID() == OrderID) return O;
        }
        return GetEmptyOrderObject();
    }

    static bool IsOrderExist(string OrderID)
    {
        return !Find(OrderID).IsEmpty();
    }

    static clsOrder GetAddNewOrderObject(string OrderID)
    {
        return clsOrder(enMode::AddNewMode, OrderID, "", "", 0, 0.0, "", enStatus::Pending);
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
        return _LoadOrdersDataFromFile();
    }
};
