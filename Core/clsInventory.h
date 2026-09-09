#pragma once
#include <iostream>
#include <vector>
#include "clsProduct.h"

using namespace std;

class clsInventory
{
public:

    static bool IncreaseStock(string ProductID, int Amount)
    {
        clsProduct Product = clsProduct::Find(ProductID);
        if(!Product.IsEmpty())
        {
            Product.SetStockQuantity(Product.StockQuantity() + Amount);
            Product.Save();
            return true;
        }
        return false;
    }

    static bool DecreaseStock(string ProductID, int Amount)
    {
        clsProduct Product = clsProduct::Find(ProductID);
        if(!Product.IsEmpty() && Product.StockQuantity() >= Amount)
        {
            Product.SetStockQuantity(Product.StockQuantity() - Amount);
            Product.Save();
            return true;
        }
        return false;
    }

    static vector<clsProduct> GetLowStockAlerts()
    {
        vector<clsProduct> vLowStock;
        vector<clsProduct> vProducts = clsProduct::GetProductsList();

        for(clsProduct& P : vProducts)
        {
            if(P.StockQuantity() <= P.MinimumStock())
            {
                vLowStock.push_back(P);
            }
        }
        return vLowStock;
    }

    static void PrintInventoryReport()
    {
        vector<clsProduct> vProducts = clsProduct::GetProductsList();
        cout << "\n\t\t\t\t\tInventory Report\n";
        cout << "_________________________________________________________________________________\n";
        for(clsProduct& P : vProducts)
        {
            cout << "Product ID: " << P.ProductID() << " | Name: " << P.Name()
                 << " | Stock: " << P.StockQuantity() << " | Min: " << P.MinimumStock() << "\n";
        }
        cout << "_________________________________________________________________________________\n";
    }
};
