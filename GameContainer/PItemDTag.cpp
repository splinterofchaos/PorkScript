//
//
//  @ Project : PorkParser
//  @ File Name : PItemDTag.cpp
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#include "PItemDTag.h"

namespace GameContainer
{
	PItemDTag::PItemDTag(string ID, string desc=""):
	itemID(ID)
	{
		itsDesc = desc;
	}

	PItemDTag::~PItemDTag()
	{
	
	}
}
