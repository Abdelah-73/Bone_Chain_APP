#pragma once
#include<iostream>
#include<string>
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

	string FirstName() const
	{
		return _FirstName;
	}

	string LastName() const
	{
		return _LastName;
	}

    string FullName() const
	{
		return _FirstName+" "+_LastName;
	}

    string Email() const
	{
		return _Gmail;
	}

	string PhoneNumber() const
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
