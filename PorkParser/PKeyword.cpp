/********************************************************
PorkScript Game Engine (C) 2012 Andy J. Brennan
All Rights Reserved.

PKeyword.cpp -- Defines PKeyword.h functions.
********************************************************/

#include "PKeyword.h"
using namespace PorkParserSpace;

string PorkParserSpace::ConvertPKeywordToString(PKeyword keyword)
{
	switch(keyword)
	{
	case REQUIRES:
		return "requires";
	case DEF:
		return "def";
	case SET:
		return "set";
	case EDEF:
		return "edef";
	case DYNAMIC:
		return "dynamic";
	case DEFAULT:
		return "default";
	case STATE:
		return "state";
	case TRUE:
		return "true";
	case FALSE:
		return "false";
	case ESET:
		return "eset";
	case RELIESON:
		return "reliesOn";
	case FROM:
		return "from";
	case TRIGGER:
		return "trigger";
	case PRESENT:
		return "present";
	case N_PRESENT:
		return "n_present";
	case INF:
		return "inf";
	case FOR:
		return "for";
	case DEFINITIVE:
		return "definitive";
	case NAME:
		return "name";
	case DESC:
		return "desc";
	case MAP:
		return "map";
	case ITEM:
		return "item";
	case ITEMLIST:
		return "itemlist";
	case ITEMDTAG:
		return "itemDTag";
	case P:
		return "p";
	case CURITEMAMT:
		return "curItemAmt";
	case IDENTIFIER:
		return "IDENTIFIER";
	case NUMCONST:
		return "NUMCONST";
	case BOOLCONST:
		return "BOOLCONST";
	case TEXTCONST:
		return "TEXTCONST";
	case QUOTE:
		return "\"";
	case OPEN_PARA:
		return "(";
	case CLOSE_PARA:
		return ")";
	case COMMA:
		return ",";
	case PERIOD:
		return ".";
	case COLON:
		return ":";
	case OPEN_BRACKET:
		return "[";
	case CLOSE_BRACKET:
		return "]";
	case PLUS:
		return "+";
	default:
		return "N/A";
	}
	return "N/A";
}