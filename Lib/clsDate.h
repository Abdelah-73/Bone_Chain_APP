#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include "clsString.h"

using namespace std;

class clsDate
{
private:
	short _Year, _Month, _Day;

public:
	clsDate()
	{
		*this = GetSystemDate();
	}

	clsDate(string Date)
	{
		*this = StringToDate(Date);
	}

	clsDate(short Day, short Month, short Year)
	{
		_Day = Day;
		_Month = Month;
		_Year = Year;
	}

	clsDate(int DayOrder, short year)
	{
		*this = DateFromDayOrderInAYear(year, DayOrder);
	}

	//------------------//

	short Day()
	{
		return _Day;
	}

	void SetDay(short Day)
	{
		_Day = Day;
	}


	short Month()
	{
		return _Month;
	}

	void SetMonth(short Month)
	{
		_Month = Month;
	}


	short Year()
	{
		return _Year;
	}

	void SetYear(short Year)
	{
		_Year = Year;
	}


	clsDate FullDate()
	{
		return *this;
	}

	void SetFullDate(clsDate date)
	{
		*this=date;
	}


	void Print()
	{
		cout << _Day << "/" << _Month << "/" << _Year << endl;
	}

	//------------------//

	static string GetSystemDateTime()
	{
		time_t now = time(0);
		tm *Time = localtime(&now);

		short Hour=Time->tm_hour;
		short Min=Time->tm_min;
		short Sec=Time->tm_sec;
		short Year=Time->tm_year+1900;
		short Month=Time->tm_mon+1;
		short Day=Time->tm_mday;



		return to_string(Day) + "/" + to_string(Month) + "/" + to_string(Year)+" - "+to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec);

	}

	static clsDate GetSystemDate()
	{
		time_t now = time(0);

		tm *ToDayDate = localtime(&now);

		clsDate CurrentDate(0, 0, 0);

		CurrentDate._Day = ToDayDate->tm_mday;
		CurrentDate._Month = ToDayDate->tm_mon + 1;
		CurrentDate._Year = ToDayDate->tm_year + 1900;

		return CurrentDate;
	}

	//------------------//

	static bool IsLeapyear(short year)
	{
		return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
	}

	bool IsLeapyear()
	{
		return IsLeapyear(_Year);
	}

	//------------------//

	static short DaysInMonth(short year, short Month)
	{
		if (Month < 1 || Month > 12)
			return 0;

		short Months[]{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		return (IsLeapyear(year) && (Month == 2)) ? 29 : Months[Month - 1];
	}

	short DaysInMonth()
	{
		return DaysInMonth(_Year,_Month);
	}

	//------------------//

	static short ReadYear()
	{
		short year;
		cout << "Enter a year :";
		cin >> year;
		while (year < 0)
		{
			cout << "Imposible,There is no negative year !" << endl;
			cout << "Type another :";
			cin >> year;
		}

		return year;
	}

	static short ReadMonth()
	{
		short month;
		cout << "Enter a month :";
		cin >> month;
		while (month < 1 || month > 12)
		{
			cout << "Incorrect Month--> [ 1<= Month <=12 ]!" << endl;
			cout << "Type another :";
			cin >> month;
		}

		return month;
	}

	static short ReadDay()
	{
		short day;
		cout << "Enter a day :";
		cin >> day;
		while (day < 1 || day > 31)
		{
			cout << "Incorrect Day--> [ 1<= Day <=31 ]!" << endl;
			cout << "Type another :";
			cin >> day;
		}

		return day;
	}

	void ReadFullDate()
	{
		_Day = ReadDay();
		_Month = ReadMonth();
		_Year = ReadYear();
	}

	//------------------//

	static short NumberOfDaysFromTheBeginingOfaYear(short year, short month, short day)
	{
		short NumberOfDays = 0;
		for (short i = 1; i < month; i++)
		{
			NumberOfDays += DaysInMonth(year, i);
		}
		return NumberOfDays + day;
	}

	static short NumberOfDaysFromTheBeginingOfaYear(clsDate date)
	{
		return NumberOfDaysFromTheBeginingOfaYear(date._Year, date._Month, date._Day);
	}

	short NumberOfDaysFromTheBeginingOfaYear()
	{
		return NumberOfDaysFromTheBeginingOfaYear(*this);
	}

	//------------------//

	static clsDate DateFromDayOrderInAYear(short year, short DayOrder)
	{
		short Month = 0, MonthDay = 0;
		while (DayOrder > MonthDay)
		{
			Month++;
			DayOrder -= MonthDay;
			MonthDay = DaysInMonth(year, Month);
		}

		return clsDate(DayOrder, Month, year);
	}

	//------------------//

	static clsDate DateAfterAddDays(clsDate date, int AddDays)
	{
		date._Day += AddDays;
		short MonthDay = DaysInMonth(date._Year, date._Month);

		while (date._Day > MonthDay)
		{
			date._Day -= MonthDay;
			date._Month++;
			if (date._Month > 12)
			{
				date._Month = 1;
				date._Year++;
			}
			MonthDay = DaysInMonth(date._Year, date._Month);
		}

		return date;
	}

	void DateAfterAddDays(int AddDays)
	{
		*this = DateAfterAddDays(*this, AddDays);
	}

	//------------------//

	static bool IsValidDate(clsDate date)
	{
		if (date._Year < 1 || date._Day < 1 || date._Month < 1 || date._Month > 12)
			return 0;

		return date._Day <= DaysInMonth(date._Year, date._Month);
	}

	bool IsValidDate()
	{
		return IsValidDate(*this);
	}

	//------------------//

	static string DateFormat(clsDate Stdate, string Format = "dd/mm/yyyy")
	{
		string FormattedDate = "";
		FormattedDate = clsString::ReplaceWord(Format, "dd", to_string(Stdate._Day));
		FormattedDate = clsString::ReplaceWord(FormattedDate, "mm", to_string(Stdate._Month));
		FormattedDate = clsString::ReplaceWord(FormattedDate, "yyyy", to_string(Stdate._Year));

		return FormattedDate;
	}

	string DateFormat(string Format = "dd/mm/yyyy")
	{
		return DateFormat(*this, Format);
	}

	//------------------//

	static bool IsDate1BeforeAnother(clsDate D1, clsDate D2)
	{
		if (D1._Year == D2._Year)
		{
			if (D1._Month == D2._Month)
			{
				if (D1._Day == D2._Day || D1._Day > D2._Day)
				{
					return 0;
				}
			}
			else if (D1._Month > D2._Month)
				return 0;
		}
		else if (D1._Year > D2._Year)
			return 0;

		return 1;
	}

	bool IsDate1BeforeAnother(clsDate D2)
	{
		return IsDate1BeforeAnother(*this, D2);
	}

	//------------------//

	static bool IsDate1AfterDate2(clsDate D1, clsDate D2)
	{
		return IsDate1BeforeAnother(D2, D1);
	}

	bool IsDate1AfterDate2(clsDate D2)
	{
		return IsDate1AfterDate2(*this, D2);
	}

	//------------------//

	static bool AreTwoDatesEqual(clsDate D1, clsDate D2)
	{
		return (D1._Year == D2._Year) ? ((D1._Month == D2._Month) ? ((D1._Day == D2._Day) ? true : false) : false) : false;
	}

	bool AreTwoDatesEqual(clsDate D2)
	{
		return AreTwoDatesEqual(*this, D2);
	}

	//------------------//

	static short CompareTwoDates(clsDate D1, clsDate D2)
	{
		return AreTwoDatesEqual(D1, D2) ? 0 : IsDate1BeforeAnother(D1, D2) ? -1
																		   : 1;
	}

	short CompareTwoDates(clsDate D2)
	{
		return CompareTwoDates(*this, D2);
	}

	//------------------//

	static bool IsDayLastInMonth(clsDate date)
	{
		return (date._Day == DaysInMonth(date._Year, date._Month));
	}

	bool IsDayLastInMonth()
	{
		return IsDayLastInMonth(*this);
	}

	//------------------//

	static bool IsMonthLastInYear(short month)
	{
		return (month == 12);
	}

	bool IsMonthLastInYear()
	{
		return (_Month == 12);
	}

	//------------------//

	static clsDate IncreaseDateByOneDay(clsDate date)
	{
		return IsDayLastInMonth(date) ? (IsMonthLastInYear(date._Month) ? clsDate(1, 1, ++date._Year) : clsDate(1, ++date._Month, date._Year)) : clsDate(++date._Day, date._Month, date._Year);
	}

	void IncreaseDateByOneDay()
	{
		*this = IncreaseDateByOneDay(*this);
	}

	//------------------//

	static clsDate IncreaseDateByXDays(clsDate date, int DaysToAdd)
	{
		for (int i = 1; i <= DaysToAdd; i++)
		{
			date = IncreaseDateByOneDay(date);
		}
		return date;
	}

	void IncreaseDateByXDays(int DaysToAdd)
	{
		for (int i = 1; i <= DaysToAdd; i++)
		{
			this->IncreaseDateByOneDay();
		}
	}

	//------------------//

	static clsDate IncreaseDateByOneWeek(clsDate date)
	{
		for (int i = 1; i <= 7; i++)
		{
			date = IncreaseDateByOneDay(date);
		}
		return date;
	}

	void IncreaseDateByOneWeek()
	{
		for (int i = 1; i <= 7; i++)
		{
			this->IncreaseDateByOneDay();
		}
	}

	//------------------//

	static clsDate IncreaseDateByXWeeks(clsDate date, int WeeksToAdd)
	{
		for (int i = 1; i <= WeeksToAdd; i++)
		{
			date = IncreaseDateByOneWeek(date);
		}
		return date;
	}

	void IncreaseDateByXWeeks(int WeeksToAdd)
	{
		for (int i = 1; i <= WeeksToAdd; i++)
		{
			this->IncreaseDateByOneWeek();
		}
	}

	//------------------//

	static clsDate IncreaseDateByOneMonth(clsDate date)
	{
		if (IsMonthLastInYear(date._Month))
		{
			date._Year++;
			date._Month = 1;
		}
		else
		{
			date._Month++;
		}

		short DaysOfMonth = DaysInMonth(date._Year, date._Month);
		if (date._Day > DaysOfMonth)
			date._Day = DaysOfMonth;

		return date;
	}

	void IncreaseDateByOneMonth()
	{
		*this = IncreaseDateByOneMonth(*this);
	}

	//------------------//

	static clsDate IncreaseDateByXMonths(clsDate date, int MonthsToAdd)
	{
		for (int i = 1; i <= MonthsToAdd; i++)
		{
			date = IncreaseDateByOneMonth(date);
		}
		return date;
	}

	void IncreaseDateByXMonths(int MonthsToAdd)
	{
		for (int i = 1; i <= MonthsToAdd; i++)
		{
			this->IncreaseDateByOneMonth();
		}
	}

	//------------------//

	static clsDate IncreaseDateByOneYear(clsDate date)
	{
		date._Year++;
		return date;
	}

	void IncreaseDateByOneYear()
	{
		_Year++;
	}

	//------------------//
	static clsDate IncreaseDateByXYears(clsDate date, int YearsToAdd)
	{
		date._Year += YearsToAdd;
		return date;
	}

	void IncreaseDateByXYears(int YearsToAdd)
	{
		_Year += YearsToAdd;
	}

	//------------------//

	static clsDate IncreaseDateByOneDecade(clsDate date)
	{
		for (int i = 1; i <= 10; i++)
		{
			date = IncreaseDateByOneYear(date);
		}
		return date;
	}

	void IncreaseDateByOneDecade()
	{
		_Year += 10;
	}

	//------------------//

	static clsDate IncreaseDateByXDecades(clsDate date, int DecadesToAdd)
	{
		date._Year += (DecadesToAdd * 10);
		return date;
	}

	void IncreaseDateByXDecades(int DecadesToAdd)
	{
		_Year += (DecadesToAdd * 10);
	}

	//------------------//

	static clsDate IncreaseDateByOneCentury(clsDate date)
	{
		date._Year += 100;
		return date;
	}

	void IncreaseDateByOneCentury()
	{
		_Year += 100;
	}
	//------------------//

	static clsDate IncreaseDateByOneMillennium(clsDate date)
	{
		date._Year += 1000;
		return date;
	}

	void IncreaseDateByOneMillennium()
	{
		_Year += 1000;
	}

	//------------------//

	static clsDate DecreaseDateByOneDay(clsDate date)
	{
		return (date._Day == 1) ? ((date._Month == 1) ? clsDate(31, 12, --date._Year) : clsDate(DaysInMonth(date._Year, --date._Month), date._Month, date._Year)) : clsDate(--date._Day, date._Month, date._Year);
	}

	void DecreaseDateByOneDay()
	{
		*this = DecreaseDateByOneDay(*this);
	}

	//------------------//

	static clsDate DecreaseDateByXDays(clsDate date, int DaysToSub)
	{
		for (int i = 1; i <= DaysToSub; i++)
		{
			date = DecreaseDateByOneDay(date);
		}
		return date;
	}

	void DecreaseDateByXDays(int DaysToSub)
	{
		for (int i = 1; i <= DaysToSub; i++)
		{
			this->DecreaseDateByOneDay();
		}
	}

	//------------------//

	static clsDate DecreaseDateByOneWeek(clsDate date)
	{
		for (int i = 1; i <= 7; i++)
		{
			date = DecreaseDateByOneDay(date);
		}
		return date;
	}

	void DecreaseDateByOneWeek()
	{
		for (int i = 1; i <= 7; i++)
		{
			this->DecreaseDateByOneDay();
		}
	}

	//------------------//

	static clsDate DecreaseDateByXWeeks(clsDate date, int WeeksToSub)
	{
		for (int i = 1; i <= WeeksToSub; i++)
		{
			date = DecreaseDateByOneWeek(date);
		}
		return date;
	}

	void DecreaseDateByXWeeks(int WeeksToSub)
	{
		for (int i = 1; i <= WeeksToSub; i++)
		{
			this->DecreaseDateByOneWeek();
		}
	}

	//------------------//

	static clsDate DecreaseDateByOneMonth(clsDate date)
	{
		if (date._Month == 1)
		{
			date._Year--;
			date._Month = 12;
		}
		else
		{
			date._Month--;
		}

		short DaysOfMonth = DaysInMonth(date._Year, date._Month);
		if (date._Day > DaysOfMonth)
			date._Day = DaysOfMonth;

		return date;
	}

	void DecreaseDateByOneMonth()
	{
		*this = DecreaseDateByOneMonth(*this);
	}

	//------------------//

	static clsDate DecreaseDateByXMonths(clsDate date, int MonthsToSub)
	{
		for (int i = 1; i <= MonthsToSub; i++)
		{
			date = DecreaseDateByOneMonth(date);
		}
		return date;
	}

	void DecreaseDateByXMonths(int MonthsToSub)
	{
		for (int i = 1; i <= MonthsToSub; i++)
		{
			this->DecreaseDateByOneMonth();
		}
	}

	//------------------//

	static clsDate DecreaseDateByOneYear(clsDate date)
	{
		date._Year--;
		return date;
	}

	void DecreaseDateByOneYear()
	{
		_Year--;
	}

	//------------------//
	static clsDate DecreaseDateByXYears(clsDate date, int YearsToSub)
	{
		date._Year -= YearsToSub;
		return date;
	}

	void DecreaseDateByXYears(int YearsToSub)
	{
		_Year -= YearsToSub;
	}

	//------------------//

	static clsDate DecreaseDateByOneDecade(clsDate date)
	{
		for (int i = 1; i <= 10; i++)
		{
			date = DecreaseDateByOneYear(date);
		}
		return date;
	}

	void DecreaseDateByOneDecade()
	{
		_Year -= 10;
	}

	//------------------//
	static clsDate DecreaseDateByXDecades(clsDate date, int DecadesToSub)
	{
		date._Year -= (DecadesToSub * 10);
		return date;
	}

	void DecreaseDateByXDecades(int DecadesToSub)
	{
		_Year -= (DecadesToSub * 10);
	}
	//------------------//

	static clsDate DecreaseDateByOneCentury(clsDate date)
	{
		date._Year -= 100;
		return date;
	}

	void DecreaseDateByOneCentury()
	{
		_Year -= 100;
	}

	//------------------//

	static clsDate DecreaseDateByOneMillennium(clsDate date)
	{
		date._Year -= 1000;
		return date;
	}

	void DecreaseDateByOneMillennium()
	{
		_Year -= 1000;
	}

	//------------------//

	static int DifferenceInDaysBetweenTwoDates(clsDate D1, clsDate D2, bool AddEndDay = 0)
	{

		int Days = 0;

		while (!AreTwoDatesEqual(D1, D2))
		{
			D1 = IncreaseDateByOneDay(D1);
			Days++;
		}

		return AddEndDay ? ++Days : Days;
	}

	int DifferenceInDaysBetweenTwoDates(clsDate D2, bool AddEndDay = 0)
	{
		return DifferenceInDaysBetweenTwoDates(*this, D2, AddEndDay);
	}

	//------------------//

	static void SwapTwoDates(clsDate &D1, clsDate &D2)
	{
		clsDate TempDate;

		TempDate._Day = D2._Day;
		TempDate._Month = D2._Month;
		TempDate._Year = D2._Year;

		D2._Day = D1._Day;
		D2._Month = D1._Month;
		D2._Year = D1._Year;

		D1._Day = TempDate._Day;
		D1._Month = TempDate._Month;
		D1._Year = TempDate._Year;
	}

	void SwapTwoDates(clsDate &D2)
	{
		SwapTwoDates(*this, D2);
	}

	//------------------//

	static int SignDifferenceInDaysBetweenTwoDates(clsDate D1, clsDate D2, bool AddEndDay = 0)
	{

		int Days = 0;

		short SignFlag = 1;

		if (!IsDate1BeforeAnother(D1, D2))
		{
			SwapTwoDates(D1, D2);
			SignFlag = -1;
		}

		while (IsDate1BeforeAnother(D1, D2))
		{
			D1 = IncreaseDateByOneDay(D1);
			Days++;
		}

		return AddEndDay ? ++Days * SignFlag : Days * SignFlag;
	}

	int SignDifferenceInDaysBetweenTwoDates(clsDate D2, bool AddEndDay = 0)
	{
		return SignDifferenceInDaysBetweenTwoDates(*this, D2, AddEndDay);
	}

	//------------------//

	static int AgeInDays(clsDate BirthDate)
	{
		return DifferenceInDaysBetweenTwoDates(BirthDate, GetSystemDate(), true);
	}

	int AgeInDays()
	{
		return AgeInDays(*this); // considering current object as a birth day
	}

	//------------------//

	static bool IsItEndOfWeek(clsDate date)
	{
		short DayOrder = DayOfWeekOrder(date);
		return (DayOrder == 6);
	}

	bool IsItEndOfWeek()
	{
		return IsItEndOfWeek(*this);
	}

	//------------------//

	static bool IsItWeekEnd(clsDate date)
	{
		short DayOrder = DayOfWeekOrder(date);
		return (DayOrder == 6 || DayOrder == 5);
	}

	bool IsItWeekEnd()
	{
		return IsItWeekEnd(*this);
	}
	//------------------//

	static bool IsBusinessDay(clsDate date)
	{
		/* short DayOrder=DayOfWeekOrder(date);
		return (DayOrder>=0&&DayOrder<=4); */
		return !IsItWeekEnd(date);
	}

	bool IsBusinessDay()
	{
		return IsBusinessDay(*this);
	}

	//------------------//

	static short DaysUntilEndOfWeek(clsDate date)
	{
		return 6 - DayOfWeekOrder(date);
	}

	short DaysUntilEndOfWeek()
	{
		return DaysUntilEndOfWeek(*this);
	}

	//------------------//

	static short DaysUntilEndOfMonth(clsDate date)
	{
		return DaysInMonth(date._Year, date._Month) - date._Day + 1;
	}

	short DaysUntilEndOfMonth()
	{
		return DaysUntilEndOfMonth(*this);
	}
	//------------------//

	static short DaysUntilEndOfYear(clsDate date)
	{
		clsDate EndOfYear;
		EndOfYear._Year = date._Year;
		EndOfYear._Month = 12;
		EndOfYear._Day = 31;

		return DifferenceInDaysBetweenTwoDates(date, EndOfYear, true);
	}

	short DaysUntilEndOfYear()
	{
		return DaysUntilEndOfYear(*this);
	}
	//------------------//

	static short CalculateActualVacation(clsDate DateFrom, clsDate DateTo)
	{
		short ActualVacationDays = 0;
		while (IsDate1BeforeAnother(DateFrom, DateTo))
		{

			if (IsBusinessDay(DateFrom))
				ActualVacationDays++;

			DateFrom = IncreaseDateByOneDay(DateFrom);
		}

		return ActualVacationDays;
	}

	short CalculateActualVacation(clsDate DateTo)
	{
		return CalculateActualVacation(*this, DateTo);
	}
	//------------------//

	static clsDate CalculateVacationEndDate(clsDate DateFrom, short VacationDays)
	{
		for (short i = 1; i <= VacationDays; i++)
		{
			if (IsItWeekEnd(DateFrom))
				i--;

			DateFrom = IncreaseDateByOneDay(DateFrom);
		}

		return DateFrom;
	}

	clsDate CalculateVacationEndDate(short VacationDays)
	{
		return CalculateVacationEndDate(*this, VacationDays);
	}
	//------------------//

	static clsDate StringToDate(string date)
	{

		vector<string> DateInfo = clsString::Split(date, "/");

		clsDate stdate(0, 0, 0);
		stdate._Day = stoi(DateInfo[0]);
		stdate._Month = stoi(DateInfo[1]);
		stdate._Year = stoi(DateInfo[2]);

		return stdate;
	}
	//------------------//

	static string DateToString(clsDate date)
	{
		return to_string(date._Day) + "/" + to_string(date._Month) + "/" + to_string(date._Year);
	}

	string DateToString()
	{
		return DateToString(*this);
	}

	//------------------//

	static int HoursInMonth(short year, short Month)
	{
		return DaysInMonth(year, Month) * 24;
	}

    int HoursInMonth()
	{
		return DaysInMonth(_Year,_Month)*24;
	}

	//------------------//

	static int MinutesInMonth(short year, short Month)
	{
		return HoursInMonth(year, Month) * 60;
	}

    int MinutesInMonth()
	{
		return HoursInMonth(_Year,_Month)*60;
	}

	//------------------//

	static int SecondsInMonth(short year, short Month)
	{
		return MinutesInMonth(year, Month) * 60;
	}

    int SecondsInMonth()
	{
		return MinutesInMonth(_Year,_Month)*60;
	}

	//------------------//


	static short DaysInYear(short year)
	{
		return (IsLeapyear(year)) ? 366 : 365;
	}

    short DaysInYear()
	{
		return DaysInYear(_Year);
	}

	//------------------//

	static int HoursInYear(short year)
	{
		return DaysInYear(year) * 24;
	}

    int HoursInYear()
	{
		return DaysInYear(_Year) * 24;
	}

	//------------------//

	static int MinutesInYear(short year)
	{
		return HoursInYear(year) * 60;
	}

    int MinutesInYear()
	{
		return HoursInYear(_Year) * 60;
	}

	//------------------//

	static int SecondsInYear(short year)
	{
		return MinutesInYear(year) * 60;
	}

    int SecondsInYear()
	{
		return MinutesInYear(_Year) * 60;
	}

	//------------------//

	static short DayOfWeekOrder(short year, short Month, short DayOfMonth)
	{
		if (Month == 2)
		{
			if (IsLeapyear(year))
			{
				if (DayOfMonth < 1 || DayOfMonth > 29)
					return -1;
			}
			else
			{
				if (DayOfMonth < 1 || DayOfMonth > 28)
					return -1;
			}
		}

		if ((Month < 1 || Month > 12) || (DayOfMonth < 1 || DayOfMonth > 31))
			return -1;

		int MonthOffsets[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

		if (Month < 3)
			year--;

		return (year + year / 4 - year / 100 + year / 400 + MonthOffsets[Month - 1] + DayOfMonth) % 7;
	}

	static short DayOfWeekOrder(clsDate date)
	{
		return DayOfWeekOrder(date._Year, date._Month, date._Day);
	}

	short DayOfWeekOrder()
	{
		return DayOfWeekOrder(*this);
	}

	//------------------//

	static string Day_ShortName(short Order)
	{
		if (Order < 0)
			return "Incorrect Date!";

		string DaysOfWeek[]{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

		return DaysOfWeek[Order];
	}

	//------------------//

	static string Month_ShortName(short Month)
	{
		if (Month < 1 || Month > 12)
			return "Incorrect Month!";

		string Months[]{"Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

		return Months[Month - 1];
	}

    string Month_ShortName()
	{
		return Month_ShortName(_Month);
	}

	//------------------//

	static void PrintMonthCalendar(short year, short Month)
	{
		cout << "\n_______________" << Month_ShortName(Month) << "_______________\n";
		cout << setw(5) << left << "Sun";
		cout << setw(5) << left << "Mon";
		cout << setw(5) << left << "Tue";
		cout << setw(5) << left << "Wed";
		cout << setw(5) << left << "Thu";
		cout << setw(5) << left << "Fri";
		cout << setw(5) << left << "Sat";
		cout << endl;

		short MonthDays = DaysInMonth(year, Month);
		short OrderOfDay = DayOfWeekOrder(year, Month, 1);

		for (short WeekDay = 0; WeekDay < OrderOfDay; WeekDay++)
			cout << setw(5) << right << "";

		for (short i = 1; i <= MonthDays; i++)
		{
			cout << setw(5) << left << i;
			if (++OrderOfDay == 7 && i < MonthDays)
			{
				OrderOfDay = 0;
				cout << endl;
			}
		}

		cout << "\n_________________________________\n";
	}

    void PrintMonthCalendar()
	{
		PrintMonthCalendar(_Year,_Month);
	}

	//------------------//

	static void PrintYearCalendar(short year)
	{
		printf("\n_________________________________\n\n%18d\n_________________________________\n", year);

		for (int month = 1; month <= 12; month++)
		{
			PrintMonthCalendar(year, month);

			cout << endl;
		}
	}

    void PrintYearCalendar()
	{
		PrintYearCalendar(_Year);
	}

};
