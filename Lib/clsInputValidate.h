#pragma once
#include <iostream>
#include <limits>
#include <string>
#include "clsDate.h"
#include "clsPeriod.h"

using namespace std;

class clsInputValidate
{
public:


	static bool IsNumberBetween(short Number, short From, short To)
	{
		return (Number >= From && Number <= To);
	}

	static bool IsNumberBetween(int Number, int From, int To)
	{
		return (Number >= From && Number <= To);
	}

	static bool IsNumberBetween(float Number, float From, float To)
	{
		return (Number >= From && Number <= To);
	}

	static bool IsNumberBetween(double Number, double From, double To)
	{
		return (Number >= From && Number <= To);
	}

	static bool IsDateBetween(clsDate Date, clsDate D1, clsDate D2)
	{
		return clsPeriod::IsDateWithinPeriod(Date, clsPeriod(D1, D2)) || clsPeriod::IsDateWithinPeriod(Date, clsPeriod(D2, D1));
	}

	static bool IsValideDate(clsDate Date)
	{
		return clsDate::IsValidDate(Date);
	}

	static bool IsWordBetween(string Word, string W1, string W2)
	{
		return (Word.compare(W1) >= 0 && Word.compare(W2) <= 0)||(Word.compare(W2) >= 0 && Word.compare(W1) <= 0);
	}

	static int ReadIntNumber(string ErrorMessage = "Invalide Input!,Enter Another :")
	{
		int Number;

		while (!(cin >> Number))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << ErrorMessage << endl;
		}
		return Number;
	}

	static int ReadIntNumberBetween(int From, int To, string ErrorMessage = "Input Out Of Range!")
	{
		int Number;
		bool IsBetween;
		do
		{
			Number = ReadIntNumber();
			IsBetween = IsNumberBetween(Number, From, To);
			if (!IsBetween)
			{
				cout << ErrorMessage << endl;
			}
		} while (!IsBetween);
		return Number;
	}

	static double ReadDblNumber(string ErrorMessage = "Invalide Input!,Enter Another :")
	{
		double Number;

		while (!(cin >> Number))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << ErrorMessage << endl;
		}
		return Number;
	}

	static double ReadDblNumberBetween(double From, double To, string ErrorMessage = "Input Out Of Range!")
	{
		double Number;
		bool IsBetween;
		do
		{
			Number = ReadDblNumber();
			IsBetween = IsNumberBetween(Number, From, To);
			if (!IsBetween)
			{
				cout << ErrorMessage << endl;
			}
		} while (!IsBetween);
		return Number;
	}

	static string ReadString(string message = "")
	{
		string Str = "";
		cout << message;
		getline(cin >> ws, Str);

		return Str;
	}

	static string ReadStringBetween(string W1, string W2, string ErrorMessage = "Input Out Of Range!")
	{
		string Str = "";
		bool IsBetween;
		do
		{
			Str = ReadString();
			IsBetween = IsWordBetween(Str, W1, W2);
			if (!IsBetween)
			{
				cout << ErrorMessage << endl;
			}
		} while (!IsBetween);
		return Str;
	}
};
