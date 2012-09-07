//
//
//  @ Project : PorkParser
//  @ File Name : PBaseWorld.cpp
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#include "PBaseWorld.h"

namespace GameContainer
{
	PBaseWorld::PBaseWorld(string name, string desc):
	PBaseEntity(name, desc)
	{
	}
	
	PBaseWorld::~PBaseWorld()
	{
	}
	
	void PBaseWorld::AddItem(string ID, UINT amt = 1, string reliesOn = "", string from = "", string condition = "")
	{
		// If item already exists in world, just increment that item's amount
		if(itsPItemList.count(ID) == 1)
		{
			++itsPItemList[ID];
		}
		else if(itsPItemList.count(ID) == 0)
		{
			PItemListItem theItem(ID, amt, reliesOn, from, condition);
			itsPItemList.insert(std::pair<string, PItemListItem>(ID, theItem));
		}
		else
			return;

		return;
	}

	void PBaseWorld::RemoveItem(string ID)
	{
		// Decrement the item's amount until it reaches 0.
		// Upon reaching zero, remove from list
		// If 'inf', just return
		if(itsPItemList.count(ID) == 1)
		{
			if(itsPItemList[ID].Infinate())
				return;

			if(itsPItemList[ID].GetAmt() > 1)
				--itsPItemList[ID];
			else if(itsPItemList[ID].GetAmt() == 1)
				itsPItemList.erase(ID);
			else
				return;
		}
		else
			return;
	}
	
	map<string,UINT> PBaseWorld::ListItems()
	{
		PItemList::iterator it = itsPItemList.begin();
		map<string,UINT> theList;
		for(UINT i=0;i<itsPItemList.size();i++,it++)
			theList.insert(pair<string,UINT>(it->first,it->second.GetAmt()));
		return theList;
	}

	PItemListItem PBaseWorld::GetItem(string ID)
	{
		if(itsPItemList.count(ID) == 1)
			return itsPItemList[ID];
		else
			throw xReturnError(ID+string(" does not exist!"));
	}

	void PBaseWorld::AddItemDTag(string itemID, string desc)
	{
		if(itsPItemDTags.count(itemID) == 0)
		{
			PItemDTag tempTag(itemID, desc);
			itsPItemDTags.insert(std::pair<string, PItemDTag>(itemID, tempTag));
		}
		else
			return;
		return;
	}

	PItemDTag PBaseWorld::GetItemDTag(string itemID)
	{
		if(itsPItemDTags.count(itemID) == 1)
			return itsPItemDTags[itemID];
		else
			throw xReturnError(itemID+" does not exist!");
	}
}