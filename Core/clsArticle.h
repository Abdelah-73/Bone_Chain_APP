#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "../Lib/clsString.h"

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

    static clsArticle _ConvertLineToArticleObject(string Line, string Separator = "#//#")
    {
        vector<string> vData = clsString::Split(Line, Separator);
        if (vData.size() == 5)
        {
            return clsArticle(enMode::UpdateMode, vData[0], vData[1], vData[2], vData[3], vData[4]);
        }
        return GetEmptyArticleObject();
    }

    static string _ConvertArticleObjectToLine(const clsArticle& Article, string Separator = "#//#")
    {
        string Record = "";
        Record += Article.ArticleID() + Separator;
        Record += Article.Title() + Separator;
        Record += Article.Category() + Separator;
        Record += Article.Content() + Separator;
        Record += Article.PublishDate();
        return Record;
    }

    static vector<clsArticle> _LoadArticlesDataFromFile(string FileName = "../Data/Articles.txt")
    {
        vector<clsArticle> vArticles;
        fstream MyFile;
        MyFile.open(FileName, ios::in);
        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                clsArticle A = _ConvertLineToArticleObject(Line);
                vArticles.push_back(A);
            }
            MyFile.close();
        }
        return vArticles;
    }

    static void _SaveArticlesDataToFile(const vector<clsArticle>& vArticles, string FileName = "../Data/Articles.txt")
    {
        fstream MyFile;
        MyFile.open(FileName, ios::out);
        if (MyFile.is_open())
        {
            for (const clsArticle& A : vArticles)
            {
                MyFile << _ConvertArticleObjectToLine(A) << endl;
            }
            MyFile.close();
        }
    }

    void _AddDataLineToFile(string Line, string FileName = "../Data/Articles.txt")
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
        vector<clsArticle> vArticles = _LoadArticlesDataFromFile();
        for (clsArticle& A : vArticles)
        {
            if (A.ArticleID() == ArticleID())
            {
                A = *this;
                break;
            }
        }
        _SaveArticlesDataToFile(vArticles);
    }

    void _AddNew()
    {
        _AddDataLineToFile(_ConvertArticleObjectToLine(*this));
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
        vector<clsArticle> vArticles = _LoadArticlesDataFromFile();
        for (const clsArticle& A : vArticles)
        {
            if (A.ArticleID() == ArticleID) return A;
        }
        return GetEmptyArticleObject();
    }

    static bool IsArticleExist(string ArticleID)
    {
        return !Find(ArticleID).IsEmpty();
    }

    static clsArticle GetAddNewArticleObject(string ArticleID)
    {
        return clsArticle(enMode::AddNewMode, ArticleID, "", "", "", "");
    }

    bool Delete()
    {
        vector<clsArticle> vArticles = _LoadArticlesDataFromFile();
        for (auto it = vArticles.begin(); it != vArticles.end(); ++it)
        {
            if (it->ArticleID() == _ArticleID)
            {
                vArticles.erase(it);
                _SaveArticlesDataToFile(vArticles);
                *this = GetEmptyArticleObject();
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

    static vector<clsArticle> GetArticlesList()
    {
        return _LoadArticlesDataFromFile();
    }
};
