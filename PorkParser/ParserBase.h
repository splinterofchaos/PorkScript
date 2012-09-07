#ifndef _PARSER_BASE_H
#define _PARSER_BASE_H

#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include "PorkParserExceptions.h"
#include "PGameContainer.h"
#include "PKeyword.h"

using GameContainer::PGameContainer;
using GameContainer::SaveToFile;
using std::ifstream;
using std::vector;
using std::multimap;

template <class KeywordType>
class ParserBase
{
public:
	ParserBase(string filename);
	virtual ~ParserBase();

protected:
	// Private member functions related to the parser
	virtual void Parse(vector<vector<KeywordType>> linesToParse) = 0;

	// Private member functions related to the Lexer
	vector<KeywordType> Lex(string line);
	virtual KeywordType Tokenize(string slice) = 0;
	
	// Private member function to remove comments from a line
	void SeperateComments(string &line);
protected:
	ifstream		itsFile;		// The file to be parsed
	string			itsFileName;	// The name of the file being parsed
	UINT*			itsCurrentLine; // Line currently being parsed (for exceptions)

	string alphaNum; // Accepted alphanumerics
	string symbols; // Accepted symbols
	string whitespace; // Accepted whitespace chars
	string acceptedChars; // Master list of accepted chars
};

// Define all member functions here.
// Due to templates, this class cannot
// have a dedicated source file.

template <class KeywordType>
ParserBase<KeywordType>::ParserBase(string filename):
itsCurrentLine(0), itsFile(filename), itsFileName(filename)
{
	alphaNum = "";
	symbols = "";
	whitespace = "";
}

template <class KeywordType>
ParserBase<KeywordType>::~ParserBase()
{
	itsFile.close();
}

template <class KeywordType>
vector<KeywordType> ParserBase<KeywordType>::Lex(string line)
{
	vector<KeywordType> tokenList;
	typedef tokenizer<boost::char_separator<char> > PTokenizer;

	boost::char_separator<char> sep(whitespace.c_str(), symbols.c_str(), boost::drop_empty_tokens);
	PTokenizer tok(line, sep);
	for(PTokenizer::iterator tok_iter=tok.begin(); tok_iter != tok.end(); ++tok_iter)
	{
		tokenList.push_back(Tokenize(*tok_iter));
	}
	
	return tokenList;
}

template <class KeywordType>
void ParserBase<KeywordType>::SeperateComments(string &line)
{
	// The line must be read for quotes to see if the hash is in quotes.
	// If a hash is in quotes it cannot be read as a comment symbol.
	// Otherwise, the first non-quoted hash will be the position at which
	// to start erasing chars to the end

	string::iterator it = line.begin();
	UINT amtOfQuotes = 0; // int to measure the amount of quotes
	UINT posOfHash; // The position of the hash being analyzed
	std::vector<UINT> posList; // List of quote positions used to determine if hash is inside them

	// Count the quotes in string
	for(UINT i=0; i<line.length(); i++, it++)
	{
		if(*it._Ptr == '"')
		{
			posList.push_back(i);
			amtOfQuotes++;
		}
	}

	// If the number of quotes is odd then there are unclosed quotes
	if(amtOfQuotes % 2 != 0)
		throw xLexicalError(*itsCurrentLine, itsFileName, "Unclosed quotes found!");

	posOfHash = line.find_first_of('#');
	// If there are no hashes, return
	if(posOfHash == line.npos)
		return;

	// If there are no quotes, delete everything to end from first hash
	if(amtOfQuotes == 0)
	{
		line.erase(posOfHash);
		return;
	}

	// By reaching here we know that there is a non-zero, even number of quotes,
	// and there are also hashes. So now we need to find if those hashes are inside of quotes.
	while(1)
	{
		UINT amtBeforeHash = 0; // Amount of quotes before hash
		// Go through our position list and find how many quotes are before current hash
		for(UINT i=0; i<posList.size(); i++)
		{
			if(posList[i] < posOfHash)
				amtBeforeHash++;
			else
			{
				// If amount of quotes is even, the hash is outside of quotes, therefore a comment
				if(amtBeforeHash % 2 == 0)
				{
					line.erase(posOfHash);
					return;
				}
				else
					break; // Hash is inside of quotes, break 'for' loop and get next hash position
			}
		}
		// Get next hash, if no other hashes exist, then there are no comments, so return
		posOfHash = line.find_first_of('#', posOfHash);
		if(posOfHash == line.npos)
			return;
	}
}

#endif //_PARSER_BASE_H