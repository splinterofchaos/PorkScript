#ifndef _PORKPARSER_H
#define _PORKPARSER_H

// The version of PorkScript this program uses:
#define PORKSCRIPT_STANDARD 1.00f

#include "ParserBase.h"
#include <PorkPy.h>
#include <stack>

namespace PorkParserSpace
{
	typedef std::pair<std::pair<string,string>,string> TempProperty;

	struct TempMItemDTagInfo
	{
		string forItem;
		string desc;
		vector<GameContainer::PItemDTagHelper> dynPieces;

		TempMItemDTagInfo()
		{
			forItem = "";
			desc = "";
		}

		// Function to erase all data inside struct
		void Zero()
		{
			forItem.erase();
			desc.erase();
			dynPieces.erase(dynPieces.begin(), dynPieces.end());
		}
	};

	struct TempMItemListInfo
	{
		string name;
		int amt;
		string reliesOn;
		string from;
		string condition;

		// Function to erase all data inside struct
		void Zero()
		{
			name.erase();
			amt = 0;
			reliesOn.erase();
			from.erase();
			condition.erase();
		}
	};

	// struct used to hold temporary information about a map state (used in TempParseInfo)
	struct TempMStateInfo
	{
		string identifier;	// Identifier of this object (i.e. its name)
		string name;		// Default name
		string desc;		// Default desc
		bool isDefinitive;
		string trigger;
		multimap<GlobalTypeEnum, TempProperty> propList; // List of all properties (name and value) for object, the key is its type
		vector<TempMItemListInfo> itemList; // List of all items
		vector<TempMItemDTagInfo> itemDTagList; // List of all itemDTags

		// Function to erase all data inside struct
		void Zero()
		{
			identifier.erase();
			name.erase();
			desc.erase();
			propList.erase(propList.begin(), propList.end());
			itemList.erase(itemList.begin(), itemList.end());
			itemDTagList.erase(itemDTagList.begin(), itemDTagList.end());
		}
	};

	// struct used to hold temporary information for the parser before it is committed to the GC
	struct TempParseInfo
	{
		ParserLevels objType;   // Type of object this structure is defining
		string identifier;		// Identifier of this object (i.e. its name)
		string name;			// Default name (if applicable)
		string desc;			// Default desc (if applicable)
		multimap<GlobalTypeEnum, TempProperty> propList; // List of all properties (name and value) for object
		vector<TempMStateInfo> stateList; // List of all states (if applicable)
		vector<TempMItemListInfo> itemList; // List of all items
		vector<TempMItemDTagInfo> itemDTagList; // List of all itemDTags

		// Function to erase all data inside struct
		void Zero()
		{
			objType = EMPTY;
			identifier.erase();
			name.erase();
			desc.erase();
			propList.erase(propList.begin(), propList.end());
			stateList.erase(stateList.begin(), stateList.end());
			itemList.erase(itemList.begin(), itemList.end());
			itemDTagList.erase(itemDTagList.begin(), itemDTagList.end());
		}
	};

	// Class PorkParser -- represents the .pmf parser + parsed information used in Pork
	class PorkParser : public ParserBase<TokenPair>
	{
	public:
		// constructors
		__declspec(dllexport) PorkParser(string filename, string name, string author);
		__declspec(dllexport) ~PorkParser();

		__declspec(dllexport) void CompilePorkScript();
		__declspec(dllexport) void SaveGCToFile() { SaveToFile(theGameContainer); }
	private:
		// Private member functions

		// Private member functions related to the parser
		void Parse(vector<vector<TokenPair>> linesToParse);
		// Function to parse arguments of variable length (such as the string input for ItemDTag)
		vector<GameContainer::PItemDTagHelper> CompressArgs(vector<TokenPair> *lineToCompress);
		// Function to check an intrinsic function call for correctness
		void FunctionCheck(GameContainer::PItemDTagHelper theFunc, vector<TokenPair> *lineOfFunc);
		// Function to prepare a python script and enter in properties
		void PreparePython(string scriptName);
		// Functions used to parse object-specific definitions
		void InsertMapDef(TempParseInfo *sInfo);
		void InsertItemDef(TempParseInfo *sInfo);

		// Private member functions related to the Lexer
		TokenPair Tokenize(string slice);
	private:
		// Private members
		PGameContainer	theGameContainer; // The GC containing all parsed information
	};
}
#endif //_PORK_PARSER_H