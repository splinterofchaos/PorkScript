//
//
//  @ Project : PorkParser
//  @ File Name : PState.cpp
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#include "PState.h"

namespace GameContainer
{
	// Regular state constructor
	PState::PState(string sID, string name, string desc, bool definitive, string trigger):
	PBaseWorld(name, desc), stateID(sID), 
	triggerName(trigger), isDefinitive(definitive)
	{
	}

	PState::~PState()
	{
	}
}
