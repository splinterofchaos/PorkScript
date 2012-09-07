#ifndef _PKEYWORD_H
#define _PKEYWORD_H

#include <string>
using std::string;

namespace PorkParserSpace
{
	// Enumeration for keywords used in PorkScript
	enum PKeyword { NONE, REQUIRES, DEF, SET, EDEF, DYNAMIC, DEFAULT, 
		STATE, TRUE, FALSE, ESET, RELIESON, FROM, TRIGGER, PRESENT,
		N_PRESENT, INF, FOR, DEFINITIVE, NAME, DESC, MAP, ITEM, 
		ITEMLIST, ITEMDTAG, P, CURITEMAMT, IDENTIFIER, NUMCONST, BOOLCONST, 
		TEXTCONST, QUOTE, OPEN_PARA, CLOSE_PARA, COMMA, PERIOD, COLON, OPEN_BRACKET,
		CLOSE_BRACKET, PLUS };

	enum ParserLevels { EMPTY, MAPDEF, ITEMDEF, IMDYNSET, MSTATESET, MITEMLISTSET };

	enum SentenceTypes { tNONE, SETDYN, SETPROP, SETDYNPROP, SETSTATE, SETITEMLIST, SETITEMDTAG, 
		DYNSETPROP, DYNSET, SETNAME, SETDESC, SETMODULE, SETDYNMODULE };

	
	typedef std::pair<PKeyword, string> TokenPair;

	// Prototype for ConvertPKeywordToString()
	string ConvertPKeywordToString(PKeyword keyword);
}

#endif //_PKEYWORD_H