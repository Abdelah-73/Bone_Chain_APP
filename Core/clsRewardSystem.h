#pragma once
#include <iostream>
#include "clsSupplier.h"

using namespace std;

class clsRewardSystem
{
public:

    static bool AddPointsToSupplier(string SupplierID, int PointsToAdd)
    {
        clsSupplier Supplier = clsSupplier::Find(SupplierID);
        if(!Supplier.IsEmpty())
        {
            Supplier.SetPoints(Supplier.Points() + PointsToAdd);
            Supplier.Save();
            return true;
        }
        return false;
    }

    static int GetSupplierPoints(string SupplierID)
    {
        clsSupplier Supplier = clsSupplier::Find(SupplierID);
        if(!Supplier.IsEmpty())
        {
            return Supplier.Points();
        }
        return 0;
    }

    // You can implement customer points tracking similarly if points exist on clsCustomer.
};
