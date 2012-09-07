//
//
//  @ Project : PorkParser
//  @ File Name : PItem.cpp
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#include "PItem.h"

namespace GameContainer
{
	PItem::PItem(string ID, string name, string desc):
	itsID(ID), PBaseEntity(name, desc)
	{
	}
	
	PItem::~PItem()
	{
	}
}
