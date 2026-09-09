#pragma once
#include <iostream>
#include <vector>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "clsDatabase.h"

using namespace std;

class clsArticle
{
private:
    enum enMode { EmptyMode = 0, UpdateMode = 1, AddNewMode = 2 };
    enMode _Mode;
    string _ArticleID;
    string _Title;
    string _Category;
    string _Content;
    string _PublishDate;

    void _Update()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("UPDATE Articles SET Title=?,Category=?,Content=?,PublishDate=? WHERE ArticleID=?");
        q.addBindValue(QString::fromStdString(_Title));
        q.addBindValue(QString::fromStdString(_Category));
        q.addBindValue(QString::fromStdString(_Content));
        q.addBindValue(QString::fromStdString(_PublishDate));
        q.addBindValue(QString::fromStdString(_ArticleID));
        q.exec();
    }

    void _AddNew()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("INSERT INTO Articles VALUES (?,?,?,?,?)");
        q.addBindValue(QString::fromStdString(_ArticleID));
        q.addBindValue(QString::fromStdString(_Title));
        q.addBindValue(QString::fromStdString(_Category));
        q.addBindValue(QString::fromStdString(_Content));
        q.addBindValue(QString::fromStdString(_PublishDate));
        q.exec();
    }

public:
    clsArticle(enMode Mode, string ArticleID, string Title, string Category, string Content, string PublishDate)
    {
        _Mode = Mode;
        _ArticleID = ArticleID;
        _Title = Title;
        _Category = Category;
        _Content = Content;
        _PublishDate = PublishDate;
    }

    bool IsEmpty() const { return _Mode == enMode::EmptyMode; }

    static clsArticle GetEmptyArticleObject()
    {
        return clsArticle(enMode::EmptyMode, "", "", "", "", "");
    }

    string ArticleID() const { return _ArticleID; }
    void SetTitle(string Title) { _Title = Title; }
    string Title() const { return _Title; }
    void SetCategory(string Category) { _Category = Category; }
    string Category() const { return _Category; }
    void SetContent(string Content) { _Content = Content; }
    string Content() const { return _Content; }
    void SetPublishDate(string PublishDate) { _PublishDate = PublishDate; }
    string PublishDate() const { return _PublishDate; }

    static clsArticle Find(string ArticleID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT * FROM Articles WHERE ArticleID=?");
        q.addBindValue(QString::fromStdString(ArticleID));
        if (q.exec() && q.next())
        {
            return clsArticle(enMode::UpdateMode,
                q.value(0).toString().toStdString(),
                q.value(1).toString().toStdString(),
                q.value(2).toString().toStdString(),
                q.value(3).toString().toStdString(),
                q.value(4).toString().toStdString());
        }
        return GetEmptyArticleObject();
    }

    static bool IsArticleExist(string ArticleID)
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("SELECT COUNT(*) FROM Articles WHERE ArticleID=?");
        q.addBindValue(QString::fromStdString(ArticleID));
        if (q.exec() && q.next())
            return q.value(0).toInt() > 0;
        return false;
    }

    static clsArticle GetAddNewArticleObject(string ArticleID)
    {
        return clsArticle(enMode::AddNewMode, ArticleID, "", "", "", "");
    }

    bool Delete()
    {
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        q.prepare("DELETE FROM Articles WHERE ArticleID=?");
        q.addBindValue(QString::fromStdString(_ArticleID));
        if (q.exec() && q.numRowsAffected() > 0)
        {
            *this = GetEmptyArticleObject();
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

    static vector<clsArticle> GetArticlesList()
    {
        vector<clsArticle> vArticles;
        QSqlQuery q(clsDatabase::GetInstance().GetDatabase());
        if (q.exec("SELECT * FROM Articles"))
        {
            while (q.next())
            {
                vArticles.push_back(clsArticle(enMode::UpdateMode,
                    q.value(0).toString().toStdString(),
                    q.value(1).toString().toStdString(),
                    q.value(2).toString().toStdString(),
                    q.value(3).toString().toStdString(),
                    q.value(4).toString().toStdString()));
            }
        }
        return vArticles;
    }
};
