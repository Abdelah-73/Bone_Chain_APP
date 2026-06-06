#pragma once
#include <iostream>
#include "clsDate.h"

using namespace std;

class clsPeriod
{

	clsDate _DateFrom;
	clsDate _DateTo;

public:

	clsPeriod()
	{
		_DateFrom = clsDate(0, 0, 0);
		_DateTo = clsDate(0, 0, 0);
	}

	clsPeriod(clsDate StartDate,clsDate EndDate)
	{
		_DateFrom=StartDate;
		_DateTo=EndDate;
	}

	clsDate StartDate()
	{
		return _DateFrom;
	}

	void SetStartDate(clsDate StartDate)
	{
		_DateFrom = StartDate;
	}

	clsDate EndDate()
	{
		return _DateTo;
	}

	void SetEndDate(clsDate EndDate)
	{
		_DateTo = EndDate;
	}

	void Print()
	{
		cout << "Start Date : ";
		_DateFrom.Print();

		cout << "\nEnd Date : ";
		_DateTo.Print();
	}

	void ReadPeriod()
	{
		cout << "Enter Start Date  :" << endl;
		_DateFrom.ReadFullDate();

		cout << endl;

		cout << "Enter End Date  :" << endl;
		_DateTo.ReadFullDate();
	}

	//------------------//

	static bool AreTowPeriodsOverlap(clsPeriod Period1, clsPeriod Period2)
	{
		return !(clsDate::IsDate1BeforeAnother(Period1._DateTo, Period2._DateFrom) || clsDate::IsDate1BeforeAnother(Period2._DateTo, Period1._DateFrom));
	}

	bool AreTowPeriodsOverlap(clsPeriod Period2)
	{
		return AreTowPeriodsOverlap(*this, Period2);
	}

	//------------------//

	static int CalculatePeriodLength(clsPeriod P1, bool AddEndDay = 0)
	{
		return clsDate::DifferenceInDaysBetweenTwoDates(P1._DateFrom, P1._DateTo, AddEndDay);
	}

	int CalculatePeriodLength(bool AddEndDay = 0)
	{
		return CalculatePeriodLength(*this, AddEndDay);
	}

	//------------------//

	static bool IsDateWithinPeriod(clsDate date, clsPeriod period)
	{
		return ((clsDate::AreTwoDatesEqual(date, period._DateFrom) || clsDate::IsDate1BeforeAnother(period._DateFrom, date)) && (clsDate::AreTwoDatesEqual(date, period._DateTo) || clsDate::IsDate1BeforeAnother(date, period._DateTo)));
	}

	bool IsDateWithinPeriod(clsDate date)
	{
		return IsDateWithinPeriod(date, *this);
	}

	//------------------//

	static int CountOverlapDays(clsPeriod Period1, clsPeriod Period2)
	{
		clsPeriod PeriodBetween;
		if (AreTowPeriodsOverlap(Period1, Period2))
		{
			if (IsDateWithinPeriod(Period1._DateFrom, Period2) && IsDateWithinPeriod(Period1._DateTo, Period2))
				PeriodBetween = Period1;
			else if (IsDateWithinPeriod(Period2._DateFrom, Period1) && IsDateWithinPeriod(Period2._DateTo, Period1))
				PeriodBetween = Period2;
			else
			{
				if (IsDateWithinPeriod(Period1._DateFrom, Period2))
				{
					PeriodBetween._DateFrom = Period1._DateFrom;
					PeriodBetween._DateTo = Period2._DateTo;
				}
				else
				{
					PeriodBetween._DateFrom = Period2._DateFrom;
					PeriodBetween._DateTo = Period1._DateTo;
				}
			}
		}
		else
			return 0;

		return CalculatePeriodLength(PeriodBetween);
	}

	int CountOverlapDays(clsPeriod Period2)
	{
		return CountOverlapDays(*this, Period2);
	}
};
