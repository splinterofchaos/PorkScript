//
//
//  @ Project : PorkParser
//  @ File Name : PItemListItem.h
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#if !defined(_PITEMLISTITEM_H)
#define _PITEMLISTITEM_H

#include "PorkString.h"
using std::string;
#include "PItem.h"

namespace GameContainer
{
	class PItemListItem
	{
	public:
		PItemListItem() {}
		PItemListItem(string ID, UINT amt, string reliesOn, string from, string condition);
		~PItemListItem();

		// theAmt value increment/decrement operators
		void operator++();
		void operator--();

		// Control the amt. and the inf. status
		void SetAsInf() { isInfinate = true; return; }
		void SetAsFinate() {isInfinate = false; return; }
		void SetAsFinate(UINT amt) { theAmt = amt; isInfinate = false; return; }
		
		// Accessors
		string GetID() const { return itemID; }
		UINT GetAmt() const { return theAmt; }
		string GetReliesOn() const { return reliesOn; }
		string GetFromState() const { return fromState; }
		string GetStateCondition() const { return stateCondition; }
		bool Infinate() const { return isInfinate; }
	private:
		template<class Archive>
		void serialize(Archive &ar, const UINT version)
		{
			ar & itemID;
			ar & theAmt;
			ar & reliesOn;
			ar & fromState;
			ar & stateCondition;
			ar & isInfinate;
		}
		friend class boost::serialization::access;

	private:
		string itemID;
		UINT theAmt;
		string reliesOn;
		string fromState;
		string stateCondition;
		bool isInfinate;
	};
}

#endif  //_PITEMLISTITEM_H
