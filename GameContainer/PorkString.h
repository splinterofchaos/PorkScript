/********************************************************
PorkScript Game Engine (C) 2012 Andy J. Brennan
All Rights Reserved.

PorkString.h -- Header file that defines std::string,
but with an inline boolean conversion op added.
********************************************************/

#ifndef _PORKSTRING_H
#define _PORKSTRING_H

#include <string>
using std::string;

inline string to_string(bool _Val)
	{	// convert boolean to string
	if(_Val == true)
		return (string("true"));
	else
		return (string("false"));
	}

#endif