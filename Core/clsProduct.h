#pragma once
#include <iostream>
#include <vector>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "clsDatabase.h"

using namespace std;

class clsProduct
{
public:
    enum enCategory { RawBoneFertilizer = 1, PowderFertilizer = 2, OrganicFertilizer = 3, FeedSupplement = 4 };

private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;
    string _ProductID;
    string _Name;
    enCategory _Category;
    double _Price;
    int _StockQuantity;
    int _MinimumStock;
    string _ProductionDate;
    string _ExpiryDate;

    void _Update()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("UPDATE Products SET Name=?,Category=?,Price=?,StockQuantity=?,"
            "MinimumStock=?,ProductionDate=?,ExpiryDate=? WHERE ProductID=?");
        q.addBindValue(QString::fromStdString(_Name));
        q.addBindValue((int)_Category);
        q.addBindValue(_Price);
        q.addBindValue(_StockQuantity);
        q.addBindValue(_MinimumStock);
        q.addBindValue(QString::fromStdString(_ProductionDate));
        q.addBindValue(QString::fromStdString(_ExpiryDate));
        q.addBindValue(QString::fromStdString(_ProductID));
        q.exec();
    }

    void _AddNew()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("INSERT INTO Products VALUES (?,?,?,?,?,?,?,?)");
        q.addBindValue(QString::fromStdString(_ProductID));
        q.addBindValue(QString::fromStdString(_Name));
        q.addBindValue((int)_Category);
        q.addBindValue(_Price);
        q.addBindValue(_StockQuantity);
        q.addBindValue(_MinimumStock);
        q.addBindValue(QString::fromStdString(_ProductionDate));
        q.addBindValue(QString::fromStdString(_ExpiryDate));
        q.exec();
    }

public:
    clsProduct(enMode Mode, string ProductID, string Name, enCategory Category, double Price, int StockQuantity, int MinimumStock, string ProductionDate, string ExpiryDate)
    {
        _Mode = Mode;
        _ProductID = ProductID;
        _Name = Name;
        _Category = Category;
        _Price = Price;
        _StockQuantity = StockQuantity;
        _MinimumStock = MinimumStock;
        _ProductionDate = ProductionDate;
        _ExpiryDate = ExpiryDate;
    }

    bool IsEmpty() const { return _Mode == enMode::EmptyMode; }

    static clsProduct GetEmptyProductObject()
    {
        return clsProduct(enMode::EmptyMode, "", "", enCategory::RawBoneFertilizer, 0.0, 0, 0, "", "");
    }

    string ProductID() const { return _ProductID; }
    void SetName(string Name) { _Name = Name; }
    string Name() const { return _Name; }
    void SetCategory(enCategory Category) { _Category = Category; }
    enCategory Category() const { return _Category; }
    void SetPrice(double Price) { _Price = Price; }
    double Price() const { return _Price; }
    void SetStockQuantity(int StockQuantity) { _StockQuantity = StockQuantity; }
    int StockQuantity() const { return _StockQuantity; }
    void SetMinimumStock(int MinimumStock) { _MinimumStock = MinimumStock; }
    int MinimumStock() const { return _MinimumStock; }
    void SetProductionDate(string ProductionDate) { _ProductionDate = ProductionDate; }
    string ProductionDate() const { return _ProductionDate; }
    void SetExpiryDate(string ExpiryDate) { _ExpiryDate = ExpiryDate; }
    string ExpiryDate() const { return _ExpiryDate; }

    static clsProduct Find(string ProductID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT * FROM Products WHERE ProductID=?");
        q.addBindValue(QString::fromStdString(ProductID));
        if (q.exec() && q.next())
        {
            return clsProduct(enMode::UpdateMode,
                q.value(0).toString().toStdString(),
                q.value(1).toString().toStdString(),
                (enCategory)q.value(2).toInt(),
                q.value(3).toDouble(),
                q.value(4).toInt(),
                q.value(5).toInt(),
                q.value(6).toString().toStdString(),
                q.value(7).toString().toStdString());
        }
        return GetEmptyProductObject();
    }

    static bool IsProductExist(string ProductID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT COUNT(*) FROM Products WHERE ProductID=?");
        q.addBindValue(QString::fromStdString(ProductID));
        if (q.exec() && q.next())
            return q.value(0).toInt() > 0;
        return false;
    }

    static clsProduct GetAddNewProductObject(string ProductID)
    {
        return clsProduct(enMode::AddNewMode, ProductID, "", enCategory::RawBoneFertilizer, 0.0, 0, 0, "", "");
    }

    bool Delete()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("DELETE FROM Products WHERE ProductID=?");
        q.addBindValue(QString::fromStdString(_ProductID));
        if (q.exec() && q.numRowsAffected() > 0)
        {
            *this = GetEmptyProductObject();
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

    static vector<clsProduct> GetProductsList()
    {
        vector<clsProduct> vProducts;
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        if (q.exec("SELECT * FROM Products"))
        {
            while (q.next())
            {
                vProducts.push_back(clsProduct(enMode::UpdateMode,
                    q.value(0).toString().toStdString(),
                    q.value(1).toString().toStdString(),
                    (enCategory)q.value(2).toInt(),
                    q.value(3).toDouble(),
                    q.value(4).toInt(),
                    q.value(5).toInt(),
                    q.value(6).toString().toStdString(),
                    q.value(7).toString().toStdString()));
            }
        }
        return vProducts;
    }
};
