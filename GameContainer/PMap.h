//
//
//  @ Project : PorkParser
//  @ File Name : PMap.h
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#if !defined(_PMAP_H)
#define _PMAP_H

#ifndef UINT
#define UINT unsigned int
#endif

#include "PorkString.h"
#include <map>
using std::map;
using std::string;
#include "GlobalExceptions.h"
#include "PBaseWorld.h"
#include "PState.h"
#include "PItemDTag.h"
#include "PItemListItem.h"

namespace GameContainer
{
	class PMap : public PBaseWorld
	{
	public:
		PMap():PBaseWorld("null", "null") {}
		PMap(string ID, string name, string desc);
		~PMap();

		// Override PBaseEntity's basic Accessors to prove this class is not abstract
		string GetName() const { return PBaseEntity::GetName(); }
		string GetDesc() const { return PBaseEntity::GetDesc(); }

		string GetID() const { return itsID; }

		// State accessors
		void SetCurrentState(string sID);
		void AddState(string sID, string name, string desc, bool isDefinitive, string trigger);
		PState *GetCurrentState();
		PState *GetState(string sID);

		// Map to state conversion operator
		operator PState();
	private:
		template<class Archive>
		void serialize(Archive &ar, const UINT version)
		{
			ar & itsID;
			ar & itsPStateList;
		}

		friend class boost::serialization::access;
	private:
		string itsID;
		string currentState; // If this string is empty, use this PMap class as the state (default)
		map<string, PState> itsPStateList; // List of the sIDs and the corresponding states
	};
}

#endif  //_PMAP_H