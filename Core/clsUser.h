#pragma once
#include <iostream>
#include <vector>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "clsPerson.h"
#include "clsDatabase.h"

using namespace std;

class clsUser : public clsPerson
{
private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;
    string _UserID;
    string _Username;
    string _Password;
    int _Role;
    bool _IsActive;
    string _SupplierID;
    string _CustomerID;

    void _Update()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("UPDATE Users SET FirstName=?,LastName=?,Email=?,Phone=?,"
            "Username=?,Password=?,Role=?,IsActive=?,SupplierID=?,CustomerID=?"
            " WHERE UserID=?");
        q.addBindValue(QString::fromStdString(FirstName()));
        q.addBindValue(QString::fromStdString(LastName()));
        q.addBindValue(QString::fromStdString(Email()));
        q.addBindValue(QString::fromStdString(PhoneNumber()));
        q.addBindValue(QString::fromStdString(_Username));
        q.addBindValue(QString::fromStdString(_Password));
        q.addBindValue(_Role);
        q.addBindValue(_IsActive ? 1 : 0);
        q.addBindValue(QString::fromStdString(_SupplierID));
        q.addBindValue(QString::fromStdString(_CustomerID));
        q.addBindValue(QString::fromStdString(_UserID));
        q.exec();
    }

    void _AddNew()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("INSERT INTO Users VALUES (?,?,?,?,?,?,?,?,?,?,?)");
        q.addBindValue(QString::fromStdString(_UserID));
        q.addBindValue(QString::fromStdString(FirstName()));
        q.addBindValue(QString::fromStdString(LastName()));
        q.addBindValue(QString::fromStdString(Email()));
        q.addBindValue(QString::fromStdString(PhoneNumber()));
        q.addBindValue(QString::fromStdString(_Username));
        q.addBindValue(QString::fromStdString(_Password));
        q.addBindValue(_Role);
        q.addBindValue(_IsActive ? 1 : 0);
        q.addBindValue(QString::fromStdString(_SupplierID));
        q.addBindValue(QString::fromStdString(_CustomerID));
        q.exec();
    }

public:
    enum enRole { Admin = 1, Supplier = 2, Customer = 3 };

    clsUser(enMode Mode, string UserID, string FirstName, string LastName, string Email, string Phone, string Username, string Password, int Role, bool IsActive, string SupplierID = "", string CustomerID = "")
        : clsPerson(FirstName, LastName, Email, Phone)
    {
        _Mode = Mode;
        _UserID = UserID;
        _Username = Username;
        _Password = Password;
        _Role = Role;
        _IsActive = IsActive;
        _SupplierID = SupplierID;
        _CustomerID = CustomerID;
    }

    bool IsEmpty() const { return _Mode == enMode::EmptyMode; }
    string SupplierID() const { return _SupplierID; }
    void SetSupplierID(string SupplierID) { _SupplierID = SupplierID; }
    string CustomerID() const { return _CustomerID; }
    void SetCustomerID(string CustomerID) { _CustomerID = CustomerID; }

    static clsUser GetEmptyUserObject()
    {
        return clsUser(enMode::EmptyMode, "", "", "", "", "", "", "", 0, false, "", "");
    }

    string UserID() const { return _UserID; }
    void SetUsername(string Username) { _Username = Username; }
    string Username() const { return _Username; }
    void SetPassword(string Password) { _Password = Password; }
    string Password() const { return _Password; }
    void SetRole(int Role) { _Role = Role; }
    int Role() const { return _Role; }
    void SetIsActive(bool IsActive) { _IsActive = IsActive; }
    bool IsActive() const { return _IsActive; }

    static clsUser Find(string UserID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT * FROM Users WHERE UserID=?");
        q.addBindValue(QString::fromStdString(UserID));
        if (q.exec() && q.next())
        {
            return clsUser(enMode::UpdateMode,
                q.value(0).toString().toStdString(),
                q.value(1).toString().toStdString(),
                q.value(2).toString().toStdString(),
                q.value(3).toString().toStdString(),
                q.value(4).toString().toStdString(),
                q.value(5).toString().toStdString(),
                q.value(6).toString().toStdString(),
                q.value(7).toInt(),
                q.value(8).toInt() != 0,
                q.value(9).toString().toStdString(),
                q.value(10).toString().toStdString());
        }
        return GetEmptyUserObject();
    }

    static clsUser FindByUsernameAndPassword(string Username, string Password)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT * FROM Users WHERE Username=? AND Password=?");
        q.addBindValue(QString::fromStdString(Username));
        q.addBindValue(QString::fromStdString(Password));
        if (q.exec() && q.next())
        {
            return clsUser(enMode::UpdateMode,
                q.value(0).toString().toStdString(),
                q.value(1).toString().toStdString(),
                q.value(2).toString().toStdString(),
                q.value(3).toString().toStdString(),
                q.value(4).toString().toStdString(),
                q.value(5).toString().toStdString(),
                q.value(6).toString().toStdString(),
                q.value(7).toInt(),
                q.value(8).toInt() != 0,
                q.value(9).toString().toStdString(),
                q.value(10).toString().toStdString());
        }
        return GetEmptyUserObject();
    }

    static bool IsUserExist(string UserID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT COUNT(*) FROM Users WHERE UserID=?");
        q.addBindValue(QString::fromStdString(UserID));
        if (q.exec() && q.next())
            return q.value(0).toInt() > 0;
        return false;
    }

    static clsUser GetAddNewUserObject(string UserID)
    {
        return clsUser(enMode::AddNewMode, UserID, "", "", "", "", "", "", 0, true, "", "");
    }

    bool Delete()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("DELETE FROM Users WHERE UserID=?");
        q.addBindValue(QString::fromStdString(_UserID));
        if (q.exec() && q.numRowsAffected() > 0)
        {
            *this = GetEmptyUserObject();
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

    static vector<clsUser> GetUsersList()
    {
        vector<clsUser> vUsers;
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        if (q.exec("SELECT * FROM Users"))
        {
            while (q.next())
            {
                vUsers.push_back(clsUser(enMode::UpdateMode,
                    q.value(0).toString().toStdString(),
                    q.value(1).toString().toStdString(),
                    q.value(2).toString().toStdString(),
                    q.value(3).toString().toStdString(),
                    q.value(4).toString().toStdString(),
                    q.value(5).toString().toStdString(),
                    q.value(6).toString().toStdString(),
                    q.value(7).toInt(),
                    q.value(8).toInt() != 0,
                    q.value(9).toString().toStdString(),
                    q.value(10).toString().toStdString()));
            }
        }
        return vUsers;
    }
};
