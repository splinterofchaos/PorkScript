//
//
//  @ Project : PorkParser
//  @ File Name : PMap.cpp
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#include "PMap.h"

namespace GameContainer
{
	PMap::PMap(string ID, string name, string desc):
	PBaseWorld(name, desc), itsID(ID)
	{
	}
	
	PMap::~PMap()
	{
	}
	
	void PMap::SetCurrentState(string sID)
	{
		if(itsPStateList.count(sID) == 1)
			currentState = sID;

		return;
	}

	void PMap::AddState(string sID, string name, string desc, bool isDefinitive = false, string trigger = "")
	{
		if(sID != "" && itsPStateList.count(sID) == 0)
		{
			PState tempState(sID, name, desc, isDefinitive, trigger);
			itsPStateList.insert(std::pair<string, PState>(sID, tempState));
		}
		return;
	}

	PState* PMap::GetCurrentState()
	{
		if(currentState == "")
			return (PState*)this;

		if(itsPStateList.count(currentState) == 1)
			return &itsPStateList[currentState];
		else
			throw xReturnError(currentState + string(" could not be found!"));

	}

	PState* PMap::GetState(string sID)
	{
		if(itsPStateList.count(sID) == 1)
			return &itsPStateList[sID];
		else
			throw xReturnError(sID + " could not be found!");
	}

	PMap::operator PState()
	{
		PState tempState("default", itsName, itsDesc, false, "");
		tempState.SetPropList(propList);
		tempState.SetDynVersions(dynVersions);
		tempState.SetPItemDTagList(itsPItemDTags);
		tempState.SetPItemList(itsPItemList);

		return tempState;
	}
}
