//
//
//  @ Project : PorkParser
//  @ File Name : PState.h
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#if !defined(_PSTATE_H)
#define _PSTATE_H

#include "PorkString.h"
using std::string;
#include "PBaseWorld.h"
#include "PItemListItem.h"
#include "PItemDTag.h"

namespace GameContainer
{
	class PState : public PBaseWorld
	{
	public:
		// Constructors
		PState():PBaseWorld("null", "null") {}
		PState(string sID, string name, string desc, bool definitive, string trigger);
		~PState();

		// Override basic Accessors of PBaseEntity to prove class is not abstract
		string GetName() const { return PBaseEntity::GetName(); }
		string GetDesc() const { return PBaseEntity::GetDesc(); }

		// Accessors
		string GetID() const { return stateID; }
		string GetTriggerName() const { return triggerName; }
		bool Defintive() const { return isDefinitive; }

		// Gross set functions
		void SetPItemDTagList(PItemDTagList newList) { itsPItemDTags = newList; }
		void SetPItemList(PItemList newList) { itsPItemList = newList; }
	private:
		template<class Archive>
		void serialize(Archive &ar, const UINT version)
		{
			ar & stateID;
			ar & triggerName;
			ar & isDefinitive;
		}
		friend class boost::serialization::access;

	private:
		string stateID; // The ID name of the state
		string triggerName;
		bool isDefinitive;
	};
}

#endif  //_PSTATE_H
