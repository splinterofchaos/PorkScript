#ifndef _PORK_PARSER_EXCEPTIONS_H
#define _PORK_PARSER_EXCEPTIONS_H

#include <string>
#include <sstream>
#include <vector>
#include "PKeyword.h"
using std::vector;
using std::string;

#ifndef UINT
#define UINT unsigned int
#endif

enum ParserType { PORK, PPF };

// Base exception class -- takes a line and a filename to log 
// where the exception was thrown in the PorkScript
class xException
{
public:
	// Constructors
	xException(UINT line, string file):lineNum(line), fileName(file), errorDesc("")
	{ 
		std::stringstream err;
		err << "Error occurred at line " << lineNum << " in file '" << fileName << "'.";
		errorDesc += err.str();
	}
	~xException() {}

	// Get the full error message
	string GetFullError() const { return errorDesc; }
	UINT GetLineNumber() const { return lineNum; }
	string GetFileName() const { return fileName; }
	
protected:
	UINT lineNum; // Line of which exception was thrown
	string fileName; // File where line is in
	string errorDesc; // Full description of error (with lineNum and fileName)
};

// Exception classes for PorkParser
class xInvalidFile: public xException
{
public:
	// Constructors
	xInvalidFile(UINT line, string file): xException(line, file) 
	{
		errorDesc.insert(0, "ERROR: FILE IS INVALID...\n");
	}
	~xInvalidFile() {}
};

class xContextError: public xException
{
public:
	xContextError(UINT line, string file, string msg):xException(line, file)
	{
		errorDesc.insert(0, string("ERROR: CONTEXT\n") + msg + string("\n") );
	}
	~xContextError() {}
};

class xLexicalError: public xException
{
public:
	xLexicalError(UINT line, string file, string msg):xException(line, file)
	{
		errorDesc.insert(0, string("ERROR: LEXICAL\n") + msg + string("\n") );
	}
	~xLexicalError() {}
};

class xSyntaxError: public xException
{
public:
	xSyntaxError(UINT line, string file, string msg, vector<PorkParserSpace::TokenPair> lexLine):xException(line, file)
	{
		itsLexLine = lexLine;
		errorDesc.insert(0, string("ERROR: SYNTAX\n") + msg + string("\n") );
		itsType = PORK;
	}
	~xSyntaxError() {}
	
	// Public function used to get the line list as a string for debugging
	string GetLexemeAsString()
	{
		string theString = "";

		if(itsType == PORK)
		{
			for(UINT i=0; i<itsLexLine.size(); i++)
			{
				theString += PorkParserSpace::ConvertPKeywordToString(itsLexLine[i].first);
				theString += " ";
			}
		}
		else
			return "INTERNAL PARSER AMBIGUITY.";

		return theString;
	}
private:
	vector<PorkParserSpace::TokenPair> itsLexLine;
	ParserType itsType;
};
#endif //_PORK_PARSER_EXCEPTIONS_H