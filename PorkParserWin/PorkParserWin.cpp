/********************************************************
PorkScript Game Engine (C) 2012 Andy J. Brennan
All Rights Reserved.

PorkParserWin.cpp -- Main Windows driver for PorkScript.
********************************************************/

#include "stdafx.h"
#include "PorkParser.h"
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using namespace PorkParserSpace;

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		PorkParser myParser("test.pmf", "The Test Adventure", "Andy Brennan");

		myParser.CompilePorkScript();

	}
	catch(xException error)
	{
		cerr << error.GetFullError() << endl;
	}
	catch(...)
	{
		cerr << "ERROR: UNIDENTIFIED ERROR OCCURRED..." << endl;
	}

	fflush(stdin);
	cout << "Press [Enter] to terminate...";
	cin.get();

	return 0;
}