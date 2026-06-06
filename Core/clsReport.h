#pragma once
#include <iostream>
#include "clsSupplier.h"
#include "clsCustomer.h"
#include "clsProduct.h"
#include "clsOrder.h"

using namespace std;

class clsReport
{
public:
    static int TotalSuppliers()
    {
        return clsSupplier::GetSuppliersList().size();
    }

    static int TotalCustomers()
    {
        return clsCustomer::GetCustomersList().size();
    }

    static int TotalProducts()
    {
        return clsProduct::GetProductsList().size();
    }

    static int TotalOrders()
    {
        return clsOrder::GetOrdersList().size();
    }

    static double TotalCollectedBones()
    {
        double Total = 0;
        vector<clsSupplier> vSuppliers = clsSupplier::GetSuppliersList();
        for(clsSupplier& S : vSuppliers)
        {
            Total += S.WeeklyQuantity();
        }
        return Total;
    }

    static double TotalRevenue()
    {
        double Total = 0;
        vector<clsOrder> vOrders = clsOrder::GetOrdersList();
        for(clsOrder& O : vOrders)
        {
            if (O.Status() == clsOrder::enStatus::Delivered || O.Status() == clsOrder::enStatus::Confirmed)
            {
                Total += O.TotalPrice();
            }
        }
        return Total;
    }

    static void PrintDashboard()
    {
        cout << "\n=============================================\n";
        cout << "\t\tSYSTEM DASHBOARD\n";
        cout << "=============================================\n";
        cout << "Total Suppliers        : " << TotalSuppliers() << "\n";
        cout << "Total Customers        : " << TotalCustomers() << "\n";
        cout << "Total Products         : " << TotalProducts() << "\n";
        cout << "Total Orders           : " << TotalOrders() << "\n";
        cout << "Total Collected Bones  : " << TotalCollectedBones() << " kg\n";
        cout << "Total Revenue          : $" << TotalRevenue() << "\n";
        cout << "=============================================\n";
    }
};
