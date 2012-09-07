#ifndef _PORKPY_EXCEPTIONS_H
#define _PORKPY_EXCEPTIONS_H

#include <string>
#include <sstream>
#include <vector>
using std::vector;
using std::string;

#ifndef UINT
#define UINT unsigned int
#endif

// Base exception class
class xException
{
public:
	// Constructors
	xException(string msg): errorDesc("")
	{ 
		std::stringstream err;
		err << "PorkPy error occurred: " << "\n\t" << msg;
		errorDesc += err.str();
	}
	~xException() {}

	// Get the full error message
	string GetFullError() const { return errorDesc; }
	
protected:
	string errorDesc; // Full description of error (with lineNum and fileName)
};
#endif //_PORKPY_EXCEPTIONS_H