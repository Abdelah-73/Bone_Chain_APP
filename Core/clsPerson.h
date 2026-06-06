#pragma once
#include<iostream>
using namespace std;


class clsPerson
{

	private:

	string _FirstName,_LastName,_Phone,_Gmail;

	public:


	clsPerson(string firstname,string lastname,string gmail,string phone)
	{
		_FirstName=firstname;
		_LastName=lastname;
		_Phone=phone;
		_Gmail=gmail;
	}

	string FirstName()
	{
		return _FirstName;
	}

	string LastName()
	{
		return _LastName;
	}

    string FullName()
	{
		return _FirstName+" "+_LastName;
	}

    string Email()
	{
		return _Gmail;
	}

	string PhoneNumber()
	{
		return _Phone;
	}


    void SetFirstName(string firstName)
	{
		_FirstName=firstName;

	}

	 void SetLastName(string lastName)
	{
		_LastName=lastName;

	}

	void SetEmail(string gmail)
	{
		_Gmail=gmail;
	}

	void SetPhone(string phone)
	{
		_Phone=phone;
	}


};
