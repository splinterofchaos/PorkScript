#include "PorkParser.h"

#pragma warning( push )
#pragma warning( disable : 4482 )

using std::getline;
using std::stack;
using namespace PorkParserSpace;

// Constructor for PorkParser - requires filename
PorkParser::PorkParser(string filename, string name, string author):
ParserBase(filename), theGameContainer(name, author, PORKSCRIPT_STANDARD)
{
	// Also construct the charlists for accepted chars

	// Alphanumerics
	// Enter in all numbers 0-9
	for(char i=0x30;i<=0x39;i++)
	{
		alphaNum += i;
	}

	// Do all uppercase chars
	for(char i=0x41;i<=0x5A;i++)
	{
		alphaNum += i;
	}

	// All lowercase
	for(char i=0x61;i<=0x7A;i++)
	{
		alphaNum += i;
	}

	// Now for all accepted symbols
	symbols.append("(),.:[]+");

	// Also enter the space and indent
	whitespace.append("\t\r\n ");

	acceptedChars = alphaNum + symbols + whitespace;
}

// Destructor
PorkParser::~PorkParser()
{
}

void PorkParser::CompilePorkScript()
{
	string lineread;
	// Set the current line being parsed to 0
	UINT lexLineNum = 0; // The line number the lexer is at
	itsCurrentLine = &lexLineNum; // Set the current line number to the lexer's line number

	if(!itsFile)
		throw xInvalidFile(NULL, itsFileName);

	// Lex each line for parser
	vector<vector<TokenPair>> lexedLines; // Vector containing all lexed lines (vector<TokenPair>)
	while(getline(itsFile, lineread))
	{
		lexLineNum++;
		// Take out the comments
		SeperateComments(lineread);
		
		// Lex the line, then check if it is empty. If so, throw it out.
		vector<TokenPair> tempTok = Lex(lineread);
		if(!tempTok.empty())
			lexedLines.push_back(tempTok);
	}

	// Pass the lexed line list into the Parser
	if(!lexedLines.empty())
		Parse(lexedLines);
	else
		throw xLexicalError(0, itsFileName, "Lexeme list is empty!");
}

void PorkParser::Parse(vector<vector<TokenPair>> linesToParse)
{
	TempParseInfo sInfo;	// Temporary info for an object (this is zeroed each time an object is 
							// fully defined and committed to the GC)
	// Temporary secondary structures for use with sInfo (after one is filled it shall be zeroed for further usage)
	TempMStateInfo sState;
	TempMItemListInfo sItem;
	TempMItemDTagInfo sDTag;
	std::pair<string,string> currentProp; // mod.prop name
	std::pair<string,string> currentDynProp;

	UINT i = 0; // The line number
	itsCurrentLine = &i;
	stack<ParserLevels> levelStack; // Stack of levels in program (mapdef, mstatedef, mitemlistdef, etc.)
	// Run through every word of every line and check the keywords for syntatical correctness
	for(; i<linesToParse.size(); i++)
	{
		bool isLastWord = false; // Bool to tell us if this is the last word in the line
		PKeyword priorWord = NONE; // The word before the current word
		SentenceTypes lineType = tNONE; // This enum will tell us what we are setting/dealing with (once we know)

		for(UINT j=0;j<linesToParse[i].size();j++)
		{
			if(j == linesToParse[i].size()-1)
				isLastWord = true;
			else
				isLastWord = false;

			// Master word table
			switch(linesToParse[i][j].first)
			{
			case REQUIRES:
				{
					if(priorWord != NONE || !levelStack.empty())
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'requires' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'requires' keyword.", linesToParse[i]);
				}
				break;
			case DEF:
				{
					if(priorWord != NONE || !levelStack.empty())
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'def' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'def' keyword.", linesToParse[i]);
				}
				break;
			case SET:
				{
					if(priorWord != NONE || levelStack.top() != MAPDEF && levelStack.top() != ITEMDEF && levelStack.top() != MSTATESET && levelStack.top() != IMDYNSET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'set' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set' keyword.", linesToParse[i]);
				}
				break;
			case ESET:
				{
					if(priorWord != NONE)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'eset' keyword.", linesToParse[i]);
					if(isLastWord)
					{
						if(levelStack.top() == IMDYNSET || levelStack.top() == MSTATESET || levelStack.top() == MITEMLISTSET)
							levelStack.pop();
						else
							throw xSyntaxError(i, itsFileName, "Unexpected usage of 'eset' keyword.", linesToParse[i]);
					}
					else
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'eset' keyword.", linesToParse[i]);
				}
				break;
			case EDEF:
				{
					if(priorWord != NONE)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'edef' keyword.", linesToParse[i]);
					if(isLastWord)
					{
						if(levelStack.top() == MAPDEF)
						{
							levelStack.pop();
							InsertMapDef(&sInfo);
							sInfo.Zero();
						}
						else if(levelStack.top() == ITEMDEF)
						{
							levelStack.pop();
							InsertItemDef(&sInfo);
							sInfo.Zero();
						}
						else
							throw xSyntaxError(i, itsFileName, "Unexpected usage of 'edef' keyword.", linesToParse[i]);
					}
					else
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'edef' keyword.", linesToParse[i]);
				}
				break;
			case DYNAMIC:
				{
					if(priorWord != SET || levelStack.top() != MAPDEF && levelStack.top() != ITEMDEF && levelStack.top() != MSTATESET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'dynamic' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
					lineType = SETDYN;
				}
				break;
			case STATE:
				{
					if(priorWord != SET || levelStack.top() != MAPDEF)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'state' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set state'.", linesToParse[i]);
					lineType = SETSTATE;
				}
				break;
			case DEFAULT:
				{
					if(levelStack.top() == MITEMLISTSET)
					{
						if(isLastWord)
							throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
						if(priorWord != FROM)
							throw xSyntaxError(i, itsFileName, "Unexpected usage of 'default' keyword.", linesToParse[i]);
						sItem.from = "default";
					}
					else if(levelStack.top() == MSTATESET)
					{
						if(isLastWord)
							throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set state'.", linesToParse[i]);
						if(priorWord != STATE)
							throw xSyntaxError(i, itsFileName, "Unexpected usage of 'default' keyword.", linesToParse[i]);
						sState.identifier = "default";
					}
					else
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'default' keyword.", linesToParse[i]);
				}
				break;
			case RELIESON:
				{
					if(priorWord != IDENTIFIER || levelStack.top() != MITEMLISTSET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'reliesOn' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
				}
				break;
			case FROM:
				{
					if(priorWord != IDENTIFIER || levelStack.top() != MITEMLISTSET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'from' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
				}
				break;
			case TRIGGER:
				{
					if(priorWord != IDENTIFIER || lineType != SETSTATE)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'trigger' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set state'.", linesToParse[i]);
				}
				break;
			case PRESENT:
				{
					if(priorWord != COMMA || levelStack.top() != MITEMLISTSET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'present' keyword.", linesToParse[i]);
					if(isLastWord)
					{
						sItem.condition = "present";
						sInfo.itemList.push_back(sItem);
						sItem.Zero();
					}
					else
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'present' keyword.", linesToParse[i]);
				}
				break;
			case N_PRESENT:
				{
					if(priorWord != COMMA || levelStack.top() != MITEMLISTSET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'present' keyword.", linesToParse[i]);
					if(isLastWord)
					{
						sItem.condition = "n_present";
						sInfo.itemList.push_back(sItem);
						sItem.Zero();
					}
					else
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'present' keyword.", linesToParse[i]);
				}
				break;
			case INF:
				{
					if(priorWord != OPEN_PARA || lineType != SETITEMLIST && levelStack.top() != MITEMLISTSET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'inf' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
					sItem.amt = 0;
				}
				break;
			case FOR:
				{
					if(priorWord != ITEMDTAG || levelStack.top() != MSTATESET && levelStack.top() != MAPDEF)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'for' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemDTag'.", linesToParse[i]);
				}
				break;
			case DEFINITIVE:
				{
					if(priorWord != STATE || levelStack.top() != MAPDEF || lineType != SETSTATE)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'definitive' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemDTag'.", linesToParse[i]);
					sState.isDefinitive = true;
				}
				break;
			case NAME:
				{
					if(priorWord != SET || levelStack.top() != ITEMDEF && levelStack.top() != MAPDEF && levelStack.top() != MSTATESET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'name' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set name'.", linesToParse[i]);
					lineType = SETNAME;
				}
				break;
			case DESC:
				{
					if(priorWord != SET || levelStack.top() != ITEMDEF && levelStack.top() != MAPDEF && levelStack.top() != MSTATESET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'desc' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set desc'.", linesToParse[i]);
					lineType = SETDESC;
				}
				break;
			case MAP:
				{
					if(priorWord != DEF || !levelStack.empty())
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'map' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'def map'.", linesToParse[i]);
					sInfo.objType = MAPDEF;
				}
				break;
			case ITEM:
				{
					if(priorWord != DEF || !levelStack.empty())
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'item' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'def item'.", linesToParse[i]);
					sInfo.objType = ITEMDEF;
				}
				break;
			case ITEMLIST:
				{
					if(priorWord != SET || levelStack.top() != MAPDEF && levelStack.top() != MSTATESET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'itemlist' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'def item'.", linesToParse[i]);
					lineType = SETITEMLIST;
				}
				break;
			case ITEMDTAG:
				{
					if(priorWord != SET || levelStack.top() != MAPDEF && levelStack.top() != MSTATESET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of 'itemDTag' keyword.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemDTag'.", linesToParse[i]);
					lineType = SETITEMDTAG;
					sDTag.dynPieces = CompressArgs(&linesToParse[i]);
				}
				break;
			case IDENTIFIER:
				{
					if(!levelStack.empty())
					{

					// Switch statement to hold all the possible places an identifier can be used
					switch(levelStack.top())
					{
					case MSTATESET:
						// Just drop into MAPDEF
					case MAPDEF:
						{
							if(lineType == tNONE)
							{
								if(priorWord != SET)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set'.", linesToParse[i]);
								lineType = SETMODULE;
								currentProp.first = linesToParse[i][j].second;
							}
							else if(lineType == SETDYN)
							{
								if(priorWord != DYNAMIC)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
								lineType = SETDYNMODULE;
								currentProp.first = linesToParse[i][j].second;
							}
							else if(lineType == SETPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::MAP, TempProperty(currentProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
							}
							else if(lineType == SETDYNPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::MAP, TempProperty(currentProp, linesToParse[i][j].second)));
							}
							else if(lineType == SETSTATE)
							{
								if(levelStack.top() == MSTATESET)
									throw xSyntaxError(i, itsFileName, "States may not be set inside of states!", linesToParse[i]);
								if(priorWord == STATE || priorWord == DEFINITIVE)
									sState.identifier = linesToParse[i][j].second;
								else if(priorWord == TRIGGER)
									sState.trigger = linesToParse[i][j].second;
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set state'.", linesToParse[i]);
							}
							else if(lineType == SETITEMLIST)
							{
								if(priorWord == ITEMLIST || priorWord == COMMA)
									sItem.name = linesToParse[i][j].second;
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
							}
							else if(lineType == SETITEMDTAG)
							{
								if(priorWord != FOR)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemDTag'.", linesToParse[i]);
								sDTag.forItem = linesToParse[i][j].second;
							}
							else if(lineType == SETMODULE)
							{
								if(priorWord != PERIOD)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set'.", linesToParse[i]);
								currentProp.second = linesToParse[i][j].second;
								lineType = SETPROP;
							}
							else if(lineType == SETDYNMODULE)
							{
								if(priorWord != PERIOD)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
								currentProp.second = linesToParse[i][j].second;
								lineType = SETDYNPROP;
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
						}
						break;
					case ITEMDEF:
						{
							if(lineType == tNONE)
							{
								if(priorWord != SET)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set'.", linesToParse[i]);
								lineType = SETMODULE;
								currentProp.first = linesToParse[i][j].second;
							}
							else if(lineType == SETDYN)
							{
								if(priorWord != DYNAMIC)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
								lineType = SETDYNMODULE;
								currentProp.first = linesToParse[i][j].second;
								currentDynProp.first = linesToParse[i][j].second;
							}
							else if(lineType == SETPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::ITEM, TempProperty(currentProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
							}
							else if(lineType == SETDYNPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::ITEM, TempProperty(currentProp, linesToParse[i][j].second)));
							}
							else if(lineType == SETMODULE)
							{
								if(priorWord != PERIOD)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set'.", linesToParse[i]);
								currentProp.second = linesToParse[i][j].second;
								lineType = SETPROP;
							}
							else if(lineType == SETDYNMODULE)
							{
								if(priorWord != PERIOD)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
								currentProp.second = linesToParse[i][j].second;
								lineType = SETDYNPROP;
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
						}
						break;
					case IMDYNSET:
						{
							if(lineType == tNONE)
							{
								if(priorWord != SET)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
								if(linesToParse[i][j].second != currentProp.second)
									throw xSyntaxError(i, itsFileName, "Unknown property set in dynamic definition.", linesToParse[i]);
								lineType = DYNSET;
							}
							else if(lineType == DYNSET)
							{
								if(priorWord != OPEN_BRACKET)
									throw xSyntaxError(i, itsFileName, "Missing '[' after identifier.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
								currentDynProp.second = currentProp.second + "[" + linesToParse[i][j].second + "]";
								lineType = DYNSETPROP;
							}
							else if(lineType == DYNSETPROP)
							{
								if(priorWord != CLOSE_BRACKET)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::ITEM, TempProperty(currentDynProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
						}
						break;
					case MITEMLISTSET:
						{
							if(priorWord == NONE)
							{
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
								sItem.name = linesToParse[i][j].second;
							}
							else if(priorWord == RELIESON)
							{
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
								sItem.reliesOn = linesToParse[i][j].second;
							}
							else if(priorWord == FROM)
							{
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
								sItem.from = linesToParse[i][j].second;
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
						}
						break;
					default:
						throw xSyntaxError(i, itsFileName, "Unexpected usage of identifier.", linesToParse[i]);
					}
					} //if(!levelStack.empty())
					else
					{
						if(priorWord == REQUIRES)
						{
							PreparePython(linesToParse[i][j].second);
						}
					}
				}
				break;
			case NUMCONST:
				{
					switch(levelStack.top())
					{
					case MSTATESET:
						// Just drop into MAPDEF
					case MAPDEF:
						{
							if(lineType == SETPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::NUM, TempProperty(currentProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
							}
							else if(lineType == SETDYNPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
								sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::NUM, TempProperty(currentProp, linesToParse[i][j].second)));
							}
							else if(lineType == SETITEMLIST)
							{
								if(priorWord == OPEN_PARA)
									sItem.amt = atoi(linesToParse[i][j].second.c_str());
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
						}
						break;
					case ITEMDEF:
						{
							if(lineType == SETPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::NUM, TempProperty(currentProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
							}
							else if(lineType == SETDYNPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
								sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::NUM, TempProperty(currentProp, linesToParse[i][j].second)));
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
						}
						break;
					case IMDYNSET:
						{
							if(lineType == DYNSETPROP)
							{
								if(priorWord != CLOSE_BRACKET)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::NUM, TempProperty(currentDynProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
						}
						break;
					case MITEMLISTSET:
						{
							if(priorWord == OPEN_PARA)
							{
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
								sItem.amt = atoi(linesToParse[i][j].second.c_str());
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
						}
						break;
					default:
						throw xSyntaxError(i, itsFileName, "Unexpected usage of numconst.", linesToParse[i]);
					}
				}
				break;
			case TEXTCONST:
				{
					switch(levelStack.top())
					{
					case MSTATESET:
						// Just drop into MAPDEF
					case MAPDEF:
						{
							if(lineType == SETPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::TEXT, TempProperty(currentProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
							}
							else if(lineType == SETNAME)
							{
								if(priorWord != NAME)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.name = linesToParse[i][j].second;
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
							}
							else if(lineType == SETDESC)
							{
								if(priorWord != DESC)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.desc = linesToParse[i][j].second;
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
							}
							else if(lineType == SETITEMDTAG)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									sDTag.desc = linesToParse[i][j].second;
							}
							else if(lineType == SETDYNPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::TEXT, TempProperty(currentProp, linesToParse[i][j].second)));
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
						}
						break;
					case ITEMDEF:
						{
							if(lineType == SETPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::TEXT, TempProperty(currentProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
							}
							else if(lineType == SETNAME)
							{
								if(priorWord != NAME)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.name = linesToParse[i][j].second;
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
							}
							else if(lineType == SETDESC)
							{
								if(priorWord != DESC)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.desc = linesToParse[i][j].second;
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
							}
							else if(lineType == SETDYNPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::TEXT, TempProperty(currentProp, linesToParse[i][j].second)));
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
						}
						break;
					case IMDYNSET:
						{
							if(lineType == DYNSETPROP)
							{
								if(priorWord != CLOSE_BRACKET)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::TEXT, TempProperty(currentDynProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
						}
						break;
					default:
						throw xSyntaxError(i, itsFileName, "Unexpected usage of textconst.", linesToParse[i]);
					}
				}
				break;
			case BOOLCONST:
				{
					switch(levelStack.top())
					{
					case MSTATESET:
						// Just drop into MAPDEF
					case MAPDEF:
						{
							if(lineType == SETPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::BOOL, TempProperty(currentProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
							}
							else if(lineType == SETDYNPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
								sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::BOOL, TempProperty(currentProp, linesToParse[i][j].second)));
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
						}
						break;
					case ITEMDEF:
						{
							if(lineType == SETPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::BOOL, TempProperty(currentProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
							}
							else if(lineType == SETDYNPROP)
							{
								if(priorWord != IDENTIFIER)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
								if(isLastWord)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
								sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::BOOL, TempProperty(currentProp, linesToParse[i][j].second)));
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
						}
						break;
					case IMDYNSET:
						{
							if(lineType == DYNSETPROP)
							{
								if(priorWord != CLOSE_BRACKET)
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
								if(isLastWord)
									sInfo.propList.insert(std::pair<GlobalTypeEnum,TempProperty>(GlobalTypeEnum::BOOL, TempProperty(currentDynProp, linesToParse[i][j].second)));
								else
									throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
							}
							else
								throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
						}
						break;
					default:
						throw xSyntaxError(i, itsFileName, "Unexpected usage of boolconst.", linesToParse[i]);
					}
				}
				break;
			case OPEN_PARA:
				{
					if(levelStack.top() == MAPDEF || levelStack.top() == ITEMDEF)
					{
						if(priorWord != IDENTIFIER)
							throw xSyntaxError(i, itsFileName, "Unexpected usage of '('.", linesToParse[i]);
						if(isLastWord)
							throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
					}
					else if(levelStack.top() == MITEMLISTSET)
					{
						if(priorWord != IDENTIFIER)
							throw xSyntaxError(i, itsFileName, "Unexpected usage of '('.", linesToParse[i]);
						if(isLastWord)
							throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set itemlist'.", linesToParse[i]);
					}
					else
						throw xSyntaxError(i, itsFileName, "Unexpected usage of '('.", linesToParse[i]);
				}
				break;
			case CLOSE_PARA:
				{
					if(levelStack.top() == MAPDEF || levelStack.top() == ITEMDEF)
					{
						if(priorWord != NUMCONST)
							throw xSyntaxError(i, itsFileName, "Unexpected usage of ')'.", linesToParse[i]);
					}
					else if(levelStack.top() == MITEMLISTSET)
					{
						if(priorWord != NUMCONST)
							throw xSyntaxError(i, itsFileName, "Unexpected usage of ')'.", linesToParse[i]);
						if(isLastWord)
						{
							sInfo.itemList.push_back(sItem);
							sItem.Zero();
						}
					}
					else
						throw xSyntaxError(i, itsFileName, "Unexpected usage of ')'.", linesToParse[i]);
				}
				break;
			case COMMA:
				{
					if(levelStack.top() != MSTATESET && levelStack.top() != MAPDEF || lineType != SETITEMLIST)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of ','.", linesToParse[i]);
					if(priorWord != CLOSE_PARA && priorWord != IDENTIFIER)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of ','.", linesToParse[i]);
					sInfo.itemList.push_back(sItem);
					sItem.Zero();
				}
				break;
			case PERIOD:
				{
					if(lineType != SETDYNMODULE && lineType != SETMODULE)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of '.'.", linesToParse[i]);
					if(priorWord != IDENTIFIER)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of '.'.", linesToParse[i]);
					if(isLastWord && lineType == SETDYNMODULE)
						throw xSyntaxError(i, itsFileName, "Unexpected of call for 'set dynamic'.", linesToParse[i]);
					if(isLastWord && lineType == SETMODULE)
						throw xSyntaxError(i, itsFileName, "Unexpected of call for 'set'.", linesToParse[i]);
				}
				break;
			case COLON:
				{
					if(!isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of ':'.", linesToParse[i]);

					if(sInfo.objType == MAPDEF)
						levelStack.push(MAPDEF);
					else if(sInfo.objType == ITEMDEF)
						levelStack.push(ITEMDEF);
					else if(sInfo.objType == NONE)
					{
						if(lineType == SETDYNPROP)
							levelStack.push(IMDYNSET);
						else if(lineType == SETITEMLIST)
						{
							if(!sInfo.itemList.empty())
								throw xSyntaxError(i, itsFileName, "Itemlist is already inline! Cannot go into 'set' block.", linesToParse[i]);
							levelStack.push(MITEMLISTSET);
						}
						else if(lineType == SETSTATE)
							levelStack.push(MSTATESET);
					}
					else
						throw xSyntaxError(i, itsFileName, "Unexpected usage of ':'.", linesToParse[i]);
				}
				break;
			case OPEN_BRACKET:
				{
					if(levelStack.top() != IMDYNSET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of '['.", linesToParse[i]);
					if(lineType != DYNSET || priorWord != IDENTIFIER)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of '['.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
				}
				break;
			case CLOSE_BRACKET:
				{
					if(levelStack.top() != IMDYNSET)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of ']'.", linesToParse[i]);
					if(lineType != DYNSETPROP || priorWord != IDENTIFIER)
						throw xSyntaxError(i, itsFileName, "Unexpected usage of ']'.", linesToParse[i]);
					if(isLastWord)
						throw xSyntaxError(i, itsFileName, "Unexpected end of call for 'set dynamic'.", linesToParse[i]);
				}
				break;
			default:
				throw xSyntaxError(i, itsFileName, "Unknown lexeme usage!", linesToParse[i]);
			}
			// Set this as the prior word for the next loop
			priorWord = linesToParse[i][j].first;
		}
	}
	return;
}

// Function to compress variable length arguments (arguments using operators such as +)
vector<GameContainer::PItemDTagHelper> PorkParser::CompressArgs(vector<TokenPair> *lineToCompress)
{
	PKeyword lvalue; // The type of the lvalue of plus
	vector<GameContainer::PItemDTagHelper> funcList;
	
	// Compress intrinsic functions into one value
	for(UINT i=0; i<lineToCompress->size(); i++)
	{
		// PItemDTagHelper struct represents our intrinsic function
		GameContainer::PItemDTagHelper tempHelper;
		// UINTs used to mark the start and end of an intrinsic function for removal from line
		UINT startOfFunc=0, endOfFunc=0;

		// If an intrinsic function is found, set its name and get args
		if(lineToCompress->at(i).first == P || lineToCompress->at(i).first == CURITEMAMT)
		{
			tempHelper.name = ConvertPKeywordToString(lineToCompress->at(i).first);
			startOfFunc = i;

			i++; // Next lexeme
			if(lineToCompress->at(i).first != OPEN_PARA)
				throw xSyntaxError(*itsCurrentLine, itsFileName, "Missing '(' after intrinsic function name.", *lineToCompress);
			i++;
			// Pick up each argument (and ensure they are comma seperated). Stop once we reach a ')'
			bool usedComma = true;
			while(lineToCompress->at(i).first != CLOSE_PARA)
			{
				if(lineToCompress->at(i).first == COMMA)
				{
					if(usedComma == true)
						throw xSyntaxError(*itsCurrentLine, itsFileName, "Comma used after another comma.", *lineToCompress);
					else
						usedComma = true;
				}
				else if(lineToCompress->at(i).first == TEXTCONST)
				{
					if(usedComma == false)
						throw xSyntaxError(*itsCurrentLine, itsFileName, "Missing comma before new argument.", *lineToCompress);
					else
						tempHelper.args.push_back(lineToCompress->at(i).second);
				}
				else
					throw xSyntaxError(*itsCurrentLine, itsFileName, "Unexpected characters used in function call.", *lineToCompress);

				i++;
			}
			endOfFunc = i;

			// Erase the function call and replace with an empty TEXTCONST
			vector<TokenPair>::iterator bIt = lineToCompress->begin(), eIt = lineToCompress->begin();
			bIt += startOfFunc;
			eIt += endOfFunc;
			lineToCompress->erase(bIt, eIt);
			lineToCompress->insert(bIt, TokenPair(TEXTCONST, ""));

			// Check the intrinsic function for correctness (exceptions will handle errors within the function)
			FunctionCheck(tempHelper, lineToCompress);
			funcList.push_back(tempHelper);
		}
	}

	UINT funcNum = 0; // Used to keep track of how many functions have been concatenated (for pos)

	// Concatenate all the added values into one, after this the function is finished
	for(UINT i=0; i<lineToCompress->size(); i++)
	{
		if(lineToCompress->at(i).first == PLUS)
		{
			if(lineToCompress->at(i-1).first == TEXTCONST)
				lvalue = TEXTCONST;
			else if(lineToCompress->at(i-1).first == NUMCONST)
				lvalue = NUMCONST;
			else if(lineToCompress->at(i-1).first == BOOLCONST)
				lvalue = BOOLCONST;
			else
				throw xSyntaxError(*itsCurrentLine, itsFileName, "Invalid type as lvalue for + operator!", *lineToCompress);
		}
		else
			continue;

		if(lineToCompress->at(i+1).first != lvalue)
			throw xSyntaxError(*itsCurrentLine, itsFileName, "rvalue of + operator does not match the lvalue!", *lineToCompress);
		else
		{
			// Create an iterator at the rvalue's position for removal
			vector<TokenPair>::iterator it = lineToCompress->begin()+i;
			
			// The rvalue is an intrinsic function, we want the position here, so get the length of the lvalue
			if(lineToCompress->at(i+1).second.empty())
			{
				if(funcNum > funcList.size())
					throw xSyntaxError(*itsCurrentLine, itsFileName, "Concatenation of empty string detected.", *lineToCompress);
				funcList[funcNum].pos = lineToCompress->at(i-1).second.length();
				funcNum++;
			}

			lineToCompress->at(i-1).second += lineToCompress->at(i+1).second;
			lineToCompress->erase(it+1); // Remove the rvalue
			lineToCompress->erase(it); // Remove the +
		}
	}
	return funcList;
}

void PorkParser::FunctionCheck(GameContainer::PItemDTagHelper theFunc, vector<TokenPair> *lineOfFunc)
{
	if(theFunc.name == "p")
	{
		if(theFunc.args.size() != 1 && theFunc.args.size() != 2)
			throw xSyntaxError(*itsCurrentLine, itsFileName, "Incorrect number of arguments for p(). p() takes either 1 or 2 args.", *lineOfFunc);
	}
	else if(theFunc.name == "curItemAmt")
	{
		if(theFunc.args.size() != 0)
			throw xSyntaxError(*itsCurrentLine, itsFileName, "Incorrect number of arguments for curItemAmt(). curItemAmt() takes no args.", *lineOfFunc);
	}
	else
		throw xSyntaxError(*itsCurrentLine, itsFileName, theFunc.name + " is not a function!", *lineOfFunc);

	return;
}

void PorkParser::PreparePython(string scriptName)
{
	// Use PorkPy to import module, get temporary identifier information
	// for ident-check later on. Upon import, each python script referenced is compiled
	// along with any libraries said module may have imported.

	// In theory, all python files should be compiled in this manner, and all .pyc
	// files will then be loaded into .zip/.7z/.lmza archive along with serialized GC
	// with AES encyption (maybe).
	

}

void PorkParser::InsertItemDef(TempParseInfo *sInfo)
{
	// Create with basic properties
	theGameContainer.CreatePItem(sInfo->identifier, sInfo->name, sInfo->desc);

	// Property list
	multimap<GlobalTypeEnum, TempProperty>::iterator it = sInfo->propList.begin();
	for(UINT i=0;i<sInfo->propList.size();i++,it++)
	{
		theGameContainer.GetPItem(sInfo->identifier)->SetCustomProp(it->second.first.second, it->second.second, it->second.first.first, it->first);
	}

	// Dynamic properties here


	return;
}

void PorkParser::InsertMapDef(TempParseInfo *sInfo)
{
	// Create with basic properties
	theGameContainer.CreatePMap(sInfo->identifier, sInfo->name, sInfo->desc);

	// Property list
	multimap<GlobalTypeEnum, TempProperty>::iterator it = sInfo->propList.begin();
	for(UINT i=0;i<sInfo->propList.size();i++,it++)
	{
		theGameContainer.GetPMap(sInfo->identifier)->SetCustomProp(it->second.first.second, it->second.second, it->second.first.first, it->first);
	}

	// Dynamic properties

	// Item list
	for(UINT i=0;i<sInfo->itemList.size();i++)
	{
		TempMItemListInfo temp = sInfo->itemList[i];
		theGameContainer.GetPMap(sInfo->identifier)->AddItem(temp.name, temp.amt, temp.reliesOn, temp.from, temp.condition);
	}

	// ItemDTags
	for(UINT i=0;i<sInfo->itemDTagList.size();i++)
	{
		TempMItemDTagInfo temp = sInfo->itemDTagList[i];
		theGameContainer.GetPMap(sInfo->identifier)->AddItemDTag(temp.forItem, temp.desc);
		for(UINT j=0;j<temp.dynPieces.size();j++)
		{
			theGameContainer.GetPMap(sInfo->identifier)->GetItemDTag(temp.forItem).AddDynPiece(temp.dynPieces[j]);
		}
	}

	// States
	for(UINT i=0;i<sInfo->stateList.size();i++)
	{
		TempMStateInfo temps = sInfo->stateList[i];
		
		// Create state with basic properties
		theGameContainer.GetPMap(sInfo->identifier)->AddState(temps.identifier, temps.name, temps.desc, temps.isDefinitive, temps.trigger);

		GameContainer::PState* state = theGameContainer.GetPMap(sInfo->identifier)->GetState(temps.identifier);

		// Property list
		multimap<GlobalTypeEnum, TempProperty>::iterator it = temps.propList.begin();
		for(UINT j=0;j<sInfo->propList.size();j++,it++)
		{
			state->SetCustomProp(it->second.first.second, it->second.second, it->second.first.first, it->first);
		}

		// Dynamic properties


		// Item list
		for(UINT j=0;j<sInfo->itemList.size();j++)
		{
			TempMItemListInfo temp = temps.itemList[j];
			state->AddItem(temp.name, temp.amt, temp.reliesOn, temp.from, temp.condition);
		}

		// ItemDTags
		for(UINT j=0;j<sInfo->itemDTagList.size();j++)
		{
			TempMItemDTagInfo temp = temps.itemDTagList[j];
			state->AddItemDTag(temp.forItem, temp.desc);
			for(UINT k=0;k<temp.dynPieces.size();k++)
			{
				state->GetItemDTag(temp.forItem).AddDynPiece(temp.dynPieces[k]);
			}
		}
	}

	return;
}

// Function that takes the string word and takes it into a token
// If the token also needs a value, that value is added to TokenPair
TokenPair PorkParser::Tokenize(string slice)
{
	// Make a list of numbers to check against (0-9)
	set<char> numList;
	for(char i=0x30;i<=0x39;i++)
		numList.insert(i);
	bool isNumConst = false;

	if(slice == "requires")
		return TokenPair(REQUIRES, "");
	else if(slice == "def")
		return TokenPair(DEF, "");
	else if(slice == "set")
		return TokenPair(SET, "");
	else if(slice == "edef")
		return TokenPair(EDEF, "");
	else if(slice == "dynamic")
		return TokenPair(DYNAMIC, "");
	else if(slice == "default")
		return TokenPair(DEFAULT, "");
	else if(slice == "state")
		return TokenPair(STATE, "");
	else if(slice == "true")
		return TokenPair(BOOLCONST, "true");
	else if(slice == "false")
		return TokenPair(BOOLCONST, "false");
	else if(slice == "eset")
		return TokenPair(ESET, "");
	else if(slice == "reliesOn")
		return TokenPair(RELIESON, "");
	else if(slice == "from")
		return TokenPair(FROM, "");
	else if(slice == "trigger")
		return TokenPair(TRIGGER, "");
	else if(slice == "present")
		return TokenPair(PRESENT, "");
	else if(slice == "n_present")
		return TokenPair(N_PRESENT, "");
	else if(slice == "inf")
		return TokenPair(INF, "");
	else if(slice == "for")
		return TokenPair(FOR, "");
	else if(slice == "definitive")
		return TokenPair(DEFINITIVE, "");
	else if(slice == "name")
		return TokenPair(NAME, "");
	else if(slice == "desc")
		return TokenPair(DESC, "");
	else if(slice == "map")
		return TokenPair(MAP, "");
	else if(slice == "item")
		return TokenPair(ITEM, "");
	else if(slice == "itemlist")
		return TokenPair(ITEMLIST, "");
	else if(slice == "itemDTag")
		return TokenPair(ITEMDTAG, "");
	else if(slice == "p")
		return TokenPair(P, "");
	else if(slice == "curItemAmt")
		return TokenPair(CURITEMAMT, "");
	else if(slice == "\"")
		return TokenPair(QUOTE, "");
	else if(slice == "(")
		return TokenPair(OPEN_PARA, "");
	else if(slice == ")")
		return TokenPair(CLOSE_PARA, "");
	else if(slice == ",")
		return TokenPair(COMMA, "");
	else if(slice == ".")
		return TokenPair(PERIOD, "");
	else if(slice == ":")
		return TokenPair(COLON, "");
	else if(slice == "[")
		return TokenPair(OPEN_BRACKET, "");
	else if(slice == "]")
		return TokenPair(CLOSE_BRACKET, "");
	else if(slice == "+")
		return TokenPair(PLUS, "");
	else
	{	// This is a text constant
		if(slice.front() == '"' && slice.back() == '"')
		{	// Also strip the quotes off
			slice.erase(slice.begin());
			slice.erase(slice.end());
			return TokenPair(TEXTCONST, slice);
		}
		else
		{
			// Not an textconst, so try numconst -- we must check if num
			isNumConst = true; // Assume it is a numconst until proven wrong
			string::iterator sIt = slice.begin();
			for(UINT i = 0; i<slice.length(); i++, sIt++)
			{
				// This char is NOT a number, therefore it is not a num const
				if(numList.count(*sIt) != 1)
				{
					isNumConst = false;
					break;
				}
			}
		if(isNumConst)
			return TokenPair(NUMCONST, slice);
		else
			// Not any type of known constant, assume identifier
			return TokenPair(IDENTIFIER, slice);
		}
	}

	// No appropiate match found, throw error
	throw xLexicalError(*itsCurrentLine, itsFileName, slice+string(" is not a token!") );
}

#pragma warning(pop)