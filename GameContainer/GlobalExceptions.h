//
//
//  @ Project : PorkParser
//  @ File Name : GlobalExceptions.h
//  @ Date : 10/10/2011
//  @ Author : Andy Brennan
//
//

#if !defined(_GLOBALEXCEPTIONS_H)
#define _GLOBALEXCEPTIONS_H

#include "PorkString.h"
using std::string;

class xGlobalException
{
public:
	// Constructors
	xGlobalException(string desc): errorDesc(desc) {}
	~xGlobalException() {}

	// Accessors
	string GetErrorDesc() const { return errorDesc; }
protected:
	string errorDesc;
};

class xReturnError: public xGlobalException
{
public:
	// Constructors
	xReturnError(string desc): xGlobalException(desc) {}
	~xReturnError() {}
};

#endif