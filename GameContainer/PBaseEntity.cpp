//
//
//  @ Project : PorkParser
//  @ File Name : PBaseEntity.cpp
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#include "PBaseEntity.h"

namespace GameContainer
{
	PBaseEntity::PBaseEntity(string name, string desc):
	itsName(name), itsDesc(desc)
	{
	}
	
	PBaseEntity::~PBaseEntity()
	{
	}

	// Set a string-typed property -- also sets map and item IDs
	void PBaseEntity::SetCustomProp(string name, string value, string reqFileName, GlobalTypeEnum type)
	{
		PropQualifier qName(reqFileName, name);

		if(propList.count(qName) == 0)
			propList.insert(pair<PropQualifier, PropValue>(qName, PropValue(type, value)));
		else
			throw xReturnError("Property '"+name+"' already defined!");

		return;
	}

	// Get a string-typed property
	string PBaseEntity::GetPropAsString(string name, string reqFileName)
	{
		PropQualifier qName(reqFileName, name);
		if(propList.count(qName) != 1)
			throw xReturnError("No property called: "+name);

		return propList[qName].second;
	}

	long PBaseEntity::GetPropAsLong(string name, string reqFileName)
	{
		PropQualifier qName(reqFileName, name);

		if(propList[qName].first != NUM)
			throw xReturnError("Property '"+name+"' is not a num!");

		if(propList.count(qName) != 1)
			throw xReturnError("No property called: "+name);

		return atol(propList[qName].second.c_str());
	}

	bool PBaseEntity::GetPropAsBool(string name, string reqFileName)
	{
		PropQualifier qName(reqFileName, name);

		if(propList[qName].first != BOOL)
			throw xReturnError("Property '"+name+"' is not a bool!");

		if(propList.count(qName) != 1)
			throw xReturnError("No property called: "+name);

		if(propList[qName].second == "true")
			return true;
		else if(propList[qName].second == "false")
			return false;
		else
			throw xReturnError("Non-boolean value stored in boolean property called: "+name);
	}

	// Dynamic property setting
	void PBaseEntity::SetDynProp(string masterProp, string reqFileName, GlobalTypeEnum identType, string dynIdentifier, string dynValue)
	{
		PropQualifier qName(reqFileName, masterProp);
		PropValue dynIdent(identType, dynIdentifier);
		PropValue dynVal(GetPropType(qName.first,qName.second), dynValue);

		if(dynVersions.count(qName) == 0)
			dynVersions.insert(pair<PropQualifier,map<PropValue,PropValue>>(qName, map<PropValue,PropValue>()));

		if(dynVersions[qName].count(dynIdent) > 0)
			throw xReturnError("Dynamic property '"+dynIdentifier+"' already defined!");

		dynVersions[qName].insert(pair<PropValue,PropValue>(dynIdent, dynVal));
		return;
	}

	bool PBaseEntity::PropertyExist(string module, string name)
	{
		if(propList.count(PropQualifier(module, name)) == 1)
			return true;
		else
			return false;
	}

	GlobalTypeEnum PBaseEntity::GetPropType(string module, string name)
	{
		PropQualifier qName(module, name);
		if(propList.count(qName) == 0)
			return NONETYPE;

		return propList[qName].first;
	}
}