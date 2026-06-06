#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class clsDelivery
{
public:
    enum enStatus { Pending = 1, Approved = 2, Rejected = 3 };

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
    // Constructor
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

    // Getters
    string DeliveryID() { return _DeliveryID; }
    string SupplierID() { return _SupplierID; }
    string ProductID() { return _ProductID; }
    string Date() { return _Date; }
    double Quantity() { return _Quantity; }
    enStatus Status() { return _Status; }

    // Setters
    void SetStatus(enStatus Status) { _Status = Status; }

    // Check Empty
    bool IsEmpty() { return _Mode == enMode::EmptyMode; }

    // --- Static Methods (Mockup for your File I/O logic) ---
    static clsDelivery GetEmptyDeliveryObject()
    {
        return clsDelivery(enMode::EmptyMode, "", "", "", "", 0, enStatus::Pending);
    }

    static clsDelivery GetAddNewDeliveryObject(string DeliveryID)
    {
        return clsDelivery(enMode::AddNewMode, DeliveryID, "", "", "", 0, enStatus::Pending);
    }

    // يجب ربط هذه الدوال بملف Deliveries.txt بنفس طريقة clsOrder
    static clsDelivery Find(string DeliveryID) {
        // Your logic to find in file...
        return GetEmptyDeliveryObject();
    }

    static vector<clsDelivery> GetDeliveriesList() {
        // Your logic to read all from file...
        vector<clsDelivery> vDeliveries;
        return vDeliveries;
    }

    void Save() {
        // Your logic to save/update to file...
    }
};
