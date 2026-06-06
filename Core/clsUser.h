#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "clsPerson.h"
#include "../Lib/clsString.h"

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

    static clsUser _ConvertLineToUserObject(string Line, string Separator = "#//#")
    {
        vector<string> vUserData = clsString::Split(Line, Separator);
        if (vUserData.size() == 9)
        {
            return clsUser(enMode::UpdateMode, vUserData[0], vUserData[1], vUserData[2], vUserData[3], vUserData[4], vUserData[5], vUserData[6], stoi(vUserData[7]), stoi(vUserData[8]));
        }
        return GetEmptyUserObject();
    }

    static string _ConvertUserObjectToLine(clsUser User, string Separator = "#//#")
    {
        string UserRecord = "";
        UserRecord += User.UserID() + Separator;
        UserRecord += User.FirstName() + Separator;
        UserRecord += User.LastName() + Separator;
        UserRecord += User.Email() + Separator;
        UserRecord += User.PhoneNumber() + Separator;
        UserRecord += User.Username() + Separator;
        UserRecord += User.Password() + Separator;
        UserRecord += to_string(User.Role()) + Separator;
        UserRecord += to_string(User.IsActive());
        return UserRecord;
    }

    static vector<clsUser> _LoadUsersDataFromFile(string FileName = "../Data/Users.txt")
    {
        vector<clsUser> vUsers;
        fstream MyFile;
        MyFile.open(FileName, ios::in);
        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                clsUser User = _ConvertLineToUserObject(Line);
                vUsers.push_back(User);
            }
            MyFile.close();
        }
        return vUsers;
    }

    static void _SaveUsersDataToFile(vector<clsUser>& vUsers, string FileName = "../Data/Users.txt")
    {
        fstream MyFile;
        MyFile.open(FileName, ios::out);
        if (MyFile.is_open())
        {
            for (clsUser& U : vUsers)
            {
                MyFile << _ConvertUserObjectToLine(U) << endl;
            }
            MyFile.close();
        }
    }

    void _AddDataLineToFile(string Line, string FileName = "../Data/Users.txt")
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
        vector<clsUser> vUsers = _LoadUsersDataFromFile();
        for (clsUser& U : vUsers)
        {
            if (U.UserID() == UserID())
            {
                U = *this;
                break;
            }
        }
        _SaveUsersDataToFile(vUsers);
    }

    void _AddNew()
    {
        _AddDataLineToFile(_ConvertUserObjectToLine(*this));
    }

public:
    enum enRole { Admin = 1, Supplier = 2, Customer = 3 };

    clsUser(enMode Mode, string UserID, string FirstName, string LastName, string Email, string Phone, string Username, string Password, int Role, bool IsActive)
        : clsPerson(FirstName, LastName, Email, Phone)
    {
        _Mode = Mode;
        _UserID = UserID;
        _Username = Username;
        _Password = Password;
        _Role = Role;
        _IsActive = IsActive;
    }

    bool IsEmpty() { return _Mode == enMode::EmptyMode; }

    static clsUser GetEmptyUserObject()
    {
        return clsUser(enMode::EmptyMode, "", "", "", "", "", "", "", 0, false);
    }

    string UserID() { return _UserID; }
    void SetUsername(string Username) { _Username = Username; }
    string Username() { return _Username; }
    void SetPassword(string Password) { _Password = Password; }
    string Password() { return _Password; }
    void SetRole(int Role) { _Role = Role; }
    int Role() { return _Role; }
    void SetIsActive(bool IsActive) { _IsActive = IsActive; }
    bool IsActive() { return _IsActive; }

    static clsUser Find(string UserID)
    {
        vector<clsUser> vUsers = _LoadUsersDataFromFile();
        for (clsUser& U : vUsers)
        {
            if (U.UserID() == UserID) return U;
        }
        return GetEmptyUserObject();
    }

    static clsUser FindByUsernameAndPassword(string Username, string Password)
    {
        vector<clsUser> vUsers = _LoadUsersDataFromFile();
        for (clsUser& U : vUsers)
        {
            if (U.Username() == Username && U.Password() == Password) return U;
        }
        return GetEmptyUserObject();
    }

    static bool IsUserExist(string UserID)
    {
        return !Find(UserID).IsEmpty();
    }

    static clsUser GetAddNewUserObject(string UserID)
    {
        return clsUser(enMode::AddNewMode, UserID, "", "", "", "", "", "", 0, true);
    }

    bool Delete()
    {
        vector<clsUser> vUsers = _LoadUsersDataFromFile();
        for (auto it = vUsers.begin(); it != vUsers.end(); ++it)
        {
            if (it->UserID() == _UserID)
            {
                vUsers.erase(it);
                _SaveUsersDataToFile(vUsers);
                *this = GetEmptyUserObject();
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

    static vector<clsUser> GetUsersList()
    {
        return _LoadUsersDataFromFile();
    }
};
