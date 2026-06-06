#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "../Lib/clsString.h"

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

    static clsProduct _ConvertLineToProductObject(string Line, string Separator = "#//#")
    {
        vector<string> vData = clsString::Split(Line, Separator);
        if (vData.size() == 8)
        {
            return clsProduct(enMode::UpdateMode, vData[0], vData[1], (enCategory)stoi(vData[2]), stod(vData[3]), stoi(vData[4]), stoi(vData[5]), vData[6], vData[7]);
        }
        return GetEmptyProductObject();
    }

    static string _ConvertProductObjectToLine(clsProduct Product, string Separator = "#//#")
    {
        string Record = "";
        Record += Product.ProductID() + Separator;
        Record += Product.Name() + Separator;
        Record += to_string(Product.Category()) + Separator;
        Record += to_string(Product.Price()) + Separator;
        Record += to_string(Product.StockQuantity()) + Separator;
        Record += to_string(Product.MinimumStock()) + Separator;
        Record += Product.ProductionDate() + Separator;
        Record += Product.ExpiryDate();
        return Record;
    }

    static vector<clsProduct> _LoadProductsDataFromFile(string FileName = "../Data/Products.txt")
    {
        vector<clsProduct> vProducts;
        fstream MyFile;
        MyFile.open(FileName, ios::in);
        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                clsProduct P = _ConvertLineToProductObject(Line);
                vProducts.push_back(P);
            }
            MyFile.close();
        }
        return vProducts;
    }

    static void _SaveProductsDataToFile(vector<clsProduct>& vProducts, string FileName = "../Data/Products.txt")
    {
        fstream MyFile;
        MyFile.open(FileName, ios::out);
        if (MyFile.is_open())
        {
            for (clsProduct& P : vProducts)
            {
                MyFile << _ConvertProductObjectToLine(P) << endl;
            }
            MyFile.close();
        }
    }

    void _AddDataLineToFile(string Line, string FileName = "../Data/Products.txt")
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
        vector<clsProduct> vProducts = _LoadProductsDataFromFile();
        for (clsProduct& P : vProducts)
        {
            if (P.ProductID() == ProductID())
            {
                P = *this;
                break;
            }
        }
        _SaveProductsDataToFile(vProducts);
    }

    void _AddNew()
    {
        _AddDataLineToFile(_ConvertProductObjectToLine(*this));
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

    bool IsEmpty() { return _Mode == enMode::EmptyMode; }

    static clsProduct GetEmptyProductObject()
    {
        return clsProduct(enMode::EmptyMode, "", "", enCategory::RawBoneFertilizer, 0.0, 0, 0, "", "");
    }

    string ProductID() { return _ProductID; }
    void SetName(string Name) { _Name = Name; }
    string Name() { return _Name; }
    void SetCategory(enCategory Category) { _Category = Category; }
    enCategory Category() { return _Category; }
    void SetPrice(double Price) { _Price = Price; }
    double Price() { return _Price; }
    void SetStockQuantity(int StockQuantity) { _StockQuantity = StockQuantity; }
    int StockQuantity() { return _StockQuantity; }
    void SetMinimumStock(int MinimumStock) { _MinimumStock = MinimumStock; }
    int MinimumStock() { return _MinimumStock; }
    void SetProductionDate(string ProductionDate) { _ProductionDate = ProductionDate; }
    string ProductionDate() { return _ProductionDate; }
    void SetExpiryDate(string ExpiryDate) { _ExpiryDate = ExpiryDate; }
    string ExpiryDate() { return _ExpiryDate; }

    static clsProduct Find(string ProductID)
    {
        vector<clsProduct> vProducts = _LoadProductsDataFromFile();
        for (clsProduct& P : vProducts)
        {
            if (P.ProductID() == ProductID) return P;
        }
        return GetEmptyProductObject();
    }

    static bool IsProductExist(string ProductID)
    {
        return !Find(ProductID).IsEmpty();
    }

    static clsProduct GetAddNewProductObject(string ProductID)
    {
        return clsProduct(enMode::AddNewMode, ProductID, "", enCategory::RawBoneFertilizer, 0.0, 0, 0, "", "");
    }

    bool Delete()
    {
        vector<clsProduct> vProducts = _LoadProductsDataFromFile();
        for (auto it = vProducts.begin(); it != vProducts.end(); ++it)
        {
            if (it->ProductID() == _ProductID)
            {
                vProducts.erase(it);
                _SaveProductsDataToFile(vProducts);
                *this = GetEmptyProductObject();
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

    static vector<clsProduct> GetProductsList()
    {
        return _LoadProductsDataFromFile();
    }
};
