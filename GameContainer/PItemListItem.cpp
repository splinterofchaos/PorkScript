//
//
//  @ Project : PorkParser
//  @ File Name : PItemListItem.cpp
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#include "PItemListItem.h"

namespace GameContainer
{
	PItemListItem::PItemListItem(string ID, UINT amt, string relies = "", string from = "", string condition = ""):
	itemID(ID), theAmt(amt), reliesOn(relies), fromState(from), stateCondition(condition)
	{
		if(theAmt <= 0)
			isInfinate = true;
		else
			isInfinate = false;
	}
	
	PItemListItem::~PItemListItem()
	{
	}

	void PItemListItem::operator++()
	{
		if(isInfinate == false)
			theAmt++;
		
		return;
	}

	void PItemListItem::operator--()
	{
		if(isInfinate == false)
			theAmt--;

		return;
	}
}
