#pragma once
#include <iostream>
#include <vector>

using namespace std;
class clsString
{
private:
	string _Value;

public:
	clsString()
	{
		_Value = "";
	}

	clsString(string Value)
	{
		_Value = Value;
	}

	void SetValue(string Value)
	{
		_Value = Value;
	}

	string GetValue()
	{
		return _Value;
	}


	//------------------//

	static string UpperLettersOfstring(string S1)
	{

		for (short i = 0; i < S1.length(); i++)
		{
			if (S1[i] != ' ')
				S1[i] = toupper(S1[i]);
		}
		return S1;
	}

	void UpperLettersOfstring()
	{

		_Value = UpperLettersOfstring(_Value);
	}

	//------------------//

	static string LowerLettersOfstring(string S1)
	{
		for (short i = 0; i < S1.length(); i++)
		{
			if (S1[i] != ' ')
				S1[i] = tolower(S1[i]);
		}
		return S1;
	}

	void LowerLettersOfstring()
	{

		_Value = LowerLettersOfstring(_Value);
	}

	//------------------//

    static	char InvertChar(char Letter)
	{
		return (Letter >= 'a' && Letter <= 'z') ? Letter - 32 : (Letter >= 'A' && Letter <= 'Z') ? Letter + 32
																								 : Letter;
	}

	//------------------//

	static string InvertLettersOfstring(string S1)
	{

		for (short i = 0; i < S1.length(); i++)
		{

			S1[i] = InvertChar(S1[i]);
		}
		return S1;
	}

	void InvertLettersOfstring()
	{
		_Value = InvertLettersOfstring(_Value);
	}

	//------------------//

	static vector<string> Split(string S1, string Delim)
	{
		vector<string> vecStrings;
		string word;
		int pos = 0;

		while ((pos = S1.find(Delim)) != std::string::npos)
		{
			word = S1.substr(0, pos);
			vecStrings.push_back(word);

			S1.erase(0, pos + Delim.length());
		}
		vecStrings.push_back(S1);

		return vecStrings;
	}

	vector<string> Split(string Delim)
	{
		return Split(_Value, Delim);
	}

	//------------------//

	static string JoinString(vector<string> Words, string Delim)
	{

		string str = "";

		vector<string>::iterator itr;
		str = Words.front();
		for (itr = Words.begin() + 1; itr < Words.end(); itr++)
		{
			str += (Delim + *itr);
		}

		return str;
	}

	static string JoinString(string Words[50],int Size, string Delim)
{

    string str = "";


    str=Words[0];
    for(int i=1;i<Size;i++)
    {
       str+=(Delim+Words[i]);
    }

  return str;
}

	//------------------//

	static string ReplaceWord(string S1, string Word1, string Word2)
	{
		int pos = 0;
		while ((pos = S1.find(Word1)) != std::string::npos)
		{
			S1 = S1.replace(pos, Word1.length(), Word2);
		}
		return S1;
	}

	string ReplaceWord(string Word1, string Word2)
	{
		return ReplaceWord(_Value, Word1, Word2);
	}

	//------------------//

    static string ReplaceWordsUsingSplit(string S1, string Word1, string Word2, bool MatchCase = true)
{
    vector<string> vecWords = Split(S1, " ");

    for (string &word : vecWords)
    {
        if (!MatchCase)
        {
            if (UpperLettersOfstring(word) == UpperLettersOfstring(Word1))
                word = Word2;
        }
        else
        {
            if (word == Word1)
                word = Word2;
        }
    }

    return JoinString(vecWords, " ");
}

    string ReplaceWordsUsingSplit(string Word1, string Word2, bool MatchCase = true)
{
   return ReplaceWordsUsingSplit(_Value,Word1,Word2,MatchCase);
}

	//------------------//

	static void PrintFirstLetterOfEachWord(string S1)
	{
		bool isFirstLetter = true;
		cout << "\nFirst letters of this string: \n";
		for (short i = 0; i < S1.length(); i++)
		{
			if (S1[i] != ' ' && isFirstLetter)
			{
				cout << S1[i] << endl;
			}
			isFirstLetter = (S1[i] == ' ' ? true : false);
		}
	}

	void PrintFirstLetterOfEachWord()
	{
		PrintFirstLetterOfEachWord(_Value);
	}

	//------------------//

	static string UpperFirstLetterOfEachWord(string S1)
	{
		bool isFirstLetter = true;
		for (short i = 0; i < S1.length(); i++)
		{
			if (S1[i] != ' ' && isFirstLetter)
			{
				S1[i] = toupper(S1[i]);
			}
			isFirstLetter = (S1[i] == ' ' ? true : false);
		}
		return S1;
	}

	void UpperFirstLetterOfEachWord()
	{
		_Value = UpperFirstLetterOfEachWord(_Value);
	}

	//------------------//

	static string  LowerFirstLetterOfEachWord(string S1)
	{
		bool isFirstLetter = true;
		for (short i = 0; i < S1.length(); i++)
		{
			if (S1[i] != ' ' && isFirstLetter)
			{
				S1[i] = tolower(S1[i]);
			}
			isFirstLetter = (S1[i] == ' ' ? true : false);
		}
		return S1;
	}

	void LowerFirstLetterOfEachWord()
	{

		_Value = LowerFirstLetterOfEachWord(_Value);
	}

	//------------------//

    static int CountCapitalLetter(string S1)
{
    int CapitalLetters = 0;
    for (short i = 0; i < S1.length(); i++)
    {

        if (S1[i] >= 'A' && S1[i] <= 'Z')
            CapitalLetters++;
    }
    return CapitalLetters;
}

    int CountCapitalLetter()
{
    return CountCapitalLetter(_Value);
}

	//------------------//

    static int CountSmallLetter(string S1)
{
    int SmallLetters = 0;
    for (short i = 0; i < S1.length(); i++)
    {

        if (S1[i] >= 'a' && S1[i] <= 'z')
            SmallLetters++;
    }
    return SmallLetters;
}

    int CountSmallLetter()
{
    return CountSmallLetter(_Value);
}

	//------------------//

enum enWhatToCount
{
    SmallLetter = 0,
    CapitalLetter = 1
};

    static short CountSmallCapitalLetters(string S1, enWhatToCount choice)
{

    int count = 0;
    for (short i = 0; i < S1.length(); i++)
    {

        if ((choice==enWhatToCount::CapitalLetter)&&(S1[i] >= 'A' && S1[i] <= 'Z'))
            count++;

        if ((choice==enWhatToCount::SmallLetter)&&(S1[i] >= 'a' && S1[i] <= 'z'))
            count++;
    }
    return count;

}

    short CountSmallCapitalLetters(enWhatToCount choice)
{

    return CountSmallCapitalLetters(_Value,choice);

}

	//------------------//

    static int CountCharInString(char character, string S1, bool Matchcase = true)
{
    int count = 0, strLen = S1.length();

    for (int i = 0; i < strLen; i++)
    {
        if (Matchcase)
        {
            if (S1[i] == character)
                count++;
        }
        else
        {
            if (tolower(S1[i]) == tolower(character))
                count++;
        }
    }

    return count;
}

    int CountCharInString(char character,bool Matchcase = true)
{
    return CountCharInString(character,_Value,Matchcase);
}

	//------------------//

    static bool IsVowel(char letter)
{
   letter=tolower(letter);
   return (letter=='a'||letter=='e'||letter=='i'||letter=='o'||letter=='u');
}

	//------------------//

    static int CountVowelsInString(string S1)
{
    int count = 0, strLen = S1.length();

    for (int i = 0; i < strLen; i++)
    {
        if(IsVowel(S1[i]))
        count++;
    }

    return count;
}

    int CountVowelsInString()
{
    return CountVowelsInString(_Value);
}

	//------------------//

    static short CountWordsOfString(string S1)
{

    string word = "", Delim = " ";
    short pos=0,count=0;


    while ((pos = S1.find(Delim)) != std::string::npos)
    {
        word = S1.substr(0, pos);
        if (!word.empty())
            count++;

        S1.erase(0, pos + Delim.length());
    }

    if (!S1.empty())
        count++;

 return count;
}

    short CountWordsOfString()
{
 return CountWordsOfString(_Value);
}

	//------------------//

    static string TrimLeft(string Str)
{
    int strlen=Str.length();
     for(int i=0;i<strlen;i++)
     {
         if(Str[i]!=' ')
         return Str.substr(i,strlen-i);
     }
  return "";
}

    void TrimLeft()
{
	_Value = TrimLeft(_Value);
}

	//------------------//

    static string TrimRight(string Str)
{
    int strlen=Str.length();
     for(int i=strlen-1;i>=0;i--)
     {
         if(Str[i]!=' ')
         return Str.substr(0,i+1);
     }
return "";
}

    void TrimRight()
{
	_Value = TrimRight(_Value);
}

	//------------------//

    static string Trim(string Str)
{
    return TrimRight(TrimLeft(Str));
}

    void Trim()
{
	_Value = Trim(_Value);
}

	//------------------//

    static string ReversWords(string S1)
{

    string word = "", Delim = " ";
    string reversedStr="";
    short pos=0;


    while ((pos = S1.find(Delim)) != std::string::npos)
    {
        word = S1.substr(0, pos+Delim.length());
        if (!word.empty())
         reversedStr=word+reversedStr;

        S1.erase(0, pos + Delim.length());
    }

    if (!S1.empty())
      reversedStr=S1+Delim+reversedStr;

 return reversedStr;
}

    void ReversWords()
{
	_Value = ReversWords(_Value);
}

	//------------------//

    static string RemovePuncts(string str)
{

    int strlen=str.length();
    string S1="";
    for(int i=0;i<strlen;i++)
    {
        if(!ispunct(str[i]))
        S1+=str[i];
    }
return S1;
}

    void RemovePuncts()
{
	_Value = RemovePuncts(_Value);
}

	//------------------//

    static int Length(string S1)
	{
      return S1.length();
	}

	int Length()
	{
		return _Value.length();
	}
};
