#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QString>
#include <QStringList>
#include <iostream>
#include "../Lib/clsString.h"

class clsDatabase
{
private:
    clsDatabase() {}
    clsDatabase(const clsDatabase&) = delete;
    clsDatabase& operator=(const clsDatabase&) = delete;

    QSqlDatabase _db;

    void _CreateTables()
    {
        QSqlQuery q(_db);

        q.exec("CREATE TABLE IF NOT EXISTS Users ("
            "UserID TEXT PRIMARY KEY,"
            "FirstName TEXT,"
            "LastName TEXT,"
            "Email TEXT,"
            "Phone TEXT,"
            "Username TEXT,"
            "Password TEXT,"
            "Role INTEGER,"
            "IsActive INTEGER,"
            "SupplierID TEXT,"
            "CustomerID TEXT"
        ")");

        q.exec("CREATE TABLE IF NOT EXISTS Suppliers ("
            "SupplierID TEXT PRIMARY KEY,"
            "Name TEXT,"
            "Phone TEXT,"
            "Address TEXT,"
            "SupplierType INTEGER,"
            "BoneType TEXT,"
            "WeeklyQuantity REAL,"
            "Points INTEGER"
        ")");

        q.exec("CREATE TABLE IF NOT EXISTS Customers ("
            "CustomerID TEXT PRIMARY KEY,"
            "Name TEXT,"
            "Phone TEXT,"
            "Address TEXT,"
            "CustomerType INTEGER,"
            "Points INTEGER"
        ")");

        q.exec("CREATE TABLE IF NOT EXISTS Products ("
            "ProductID TEXT PRIMARY KEY,"
            "Name TEXT,"
            "Category INTEGER,"
            "Price REAL,"
            "StockQuantity INTEGER,"
            "MinimumStock INTEGER,"
            "ProductionDate TEXT,"
            "ExpiryDate TEXT"
        ")");

        q.exec("CREATE TABLE IF NOT EXISTS Orders ("
            "OrderID TEXT PRIMARY KEY,"
            "CustomerID TEXT,"
            "ProductID TEXT,"
            "Quantity INTEGER,"
            "TotalPrice REAL,"
            "OrderDate TEXT,"
            "Status INTEGER,"
            "PointsAwarded INTEGER"
        ")");

        q.exec("CREATE TABLE IF NOT EXISTS Deliveries ("
            "DeliveryID TEXT PRIMARY KEY,"
            "SupplierID TEXT,"
            "ProductID TEXT,"
            "Date TEXT,"
            "Quantity REAL,"
            "Status INTEGER"
        ")");

        q.exec("CREATE TABLE IF NOT EXISTS Articles ("
            "ArticleID TEXT PRIMARY KEY,"
            "Title TEXT,"
            "Category TEXT,"
            "Content TEXT,"
            "PublishDate TEXT"
        ")");
    }

    void _MigrateTable(const QString& filename, const QString& table, int columnCount,
        std::function<QStringList(const QStringList&)> buildValues)
    {
        QFile file(filename);
        if (!file.exists()) return;

        QSqlQuery check(_db);
        check.exec(QString("SELECT COUNT(*) FROM %1").arg(table));
        if (check.next() && check.value(0).toInt() > 0) return;

        QSqlQuery q(_db);
        _db.transaction();

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;

            QString separator = "#//#";
            QStringList parts = line.split(separator, Qt::KeepEmptyParts);
            if (parts.size() < 1) continue;

            QStringList values = buildValues(parts);
            if (values.size() != columnCount) continue;

            QStringList placeholders;
            for (int i = 0; i < columnCount; ++i)
                placeholders << "?";
            QString sql = QString("INSERT INTO %1 VALUES (%2)")
                .arg(table, placeholders.join(","));

            QSqlQuery ins(_db);
            ins.prepare(sql);
            for (int i = 0; i < columnCount; ++i)
                ins.bindValue(i, values[i]);
            if (!ins.exec())
                std::cerr << "Migration insert failed: " << ins.lastError().text().toStdString() << "\n";
        }
        file.close();
        _db.commit();
    }

    void _Migrate()
    {
        _MigrateTable("../Data/Users.txt", "Users", 11,
            [](const QStringList& p) -> QStringList {
                if (p.size() < 9) return {};
                QStringList v;
                v << p[0] << p[1] << p[2] << p[3] << p[4] << p[5] << p[6] << p[7] << p[8];
                v << (p.size() > 9 ? p[9] : QString()) << (p.size() > 10 ? p[10] : QString());
                return v;
            });

        _MigrateTable("../Data/Suppliers.txt", "Suppliers", 8,
            [](const QStringList& p) -> QStringList {
                if (p.size() < 8) return {};
                QStringList v;
                v << p[0] << p[1] << p[2] << p[3] << p[4] << p[5] << p[6] << p[7];
                return v;
            });

        _MigrateTable("../Data/Customers.txt", "Customers", 6,
            [](const QStringList& p) -> QStringList {
                if (p.size() < 5) return {};
                QStringList v;
                v << p[0] << p[1] << p[2] << p[3] << p[4];
                v << (p.size() > 5 ? p[5] : "0");
                return v;
            });

        _MigrateTable("../Data/Products.txt", "Products", 8,
            [](const QStringList& p) -> QStringList {
                if (p.size() < 8) return {};
                QStringList v;
                v << p[0] << p[1] << p[2] << p[3] << p[4] << p[5] << p[6] << p[7];
                return v;
            });

        _MigrateTable("../Data/Orders.txt", "Orders", 8,
            [](const QStringList& p) -> QStringList {
                if (p.size() < 7) return {};
                QStringList v;
                v << p[0] << p[1] << p[2] << p[3] << p[4] << p[5] << p[6];
                v << (p.size() > 7 ? p[7] : "0");
                return v;
            });

        _MigrateTable("../Data/Deliveries.txt", "Deliveries", 6,
            [](const QStringList& p) -> QStringList {
                if (p.size() < 6) return {};
                QStringList v;
                v << p[0] << p[1] << p[2] << p[3] << p[4] << p[5];
                return v;
            });

        _MigrateTable("../Data/Articles.txt", "Articles", 5,
            [](const QStringList& p) -> QStringList {
                if (p.size() < 5) return {};
                QStringList v;
                v << p[0] << p[1] << p[2] << p[3] << p[4];
                return v;
            });
    }

public:
    static clsDatabase& GetInstance()
    {
        static clsDatabase instance;
        return instance;
    }

    QSqlDatabase& GetDatabase()
    {
        return _db;
    }

    bool Initialize()
    {
        _db = QSqlDatabase::addDatabase("QSQLITE");
        _db.setDatabaseName("../Data/BoneChain.db");

        if (!_db.open())
        {
            std::cerr << "Failed to open database: " << _db.lastError().text().toStdString() << "\n";
            return false;
        }

        _CreateTables();
        _Migrate();
        return true;
    }
};
