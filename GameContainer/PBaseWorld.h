//
//
//  @ Project : PorkParser
//  @ File Name : PBaseWorld.h
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#if !defined(_PBASEWORLD_H)
#define _PBASEWORLD_H

#ifndef UINT
#define UINT unsigned int
#endif

#include "PorkString.h"
#include <boost/serialization/map.hpp>
using std::map;
using std::string;
#include "PBaseEntity.h"
#include "PItemListItem.h"
#include "PItemDTag.h"
#include "GlobalExceptions.h"

namespace GameContainer
{

	typedef map<string, PItemDTag> PItemDTagList;
	typedef map<string, PItemListItem> PItemList;

	class PBaseWorld : public PBaseEntity
	{
	public:
		// Constructors
		PBaseWorld(string name, string desc);
		virtual ~PBaseWorld();

		// Accessors
		void AddItem(string ID, UINT amt, string reliesOn, string from, string condition);
		void RemoveItem(string ID);
		map<string, UINT> ListItems();
		PItemListItem GetItem(string ID);

		void AddItemDTag(string itemID, string desc);
		PItemDTag GetItemDTag(string itemID);
	private:
		template<class Archive>
		void serialize(Archive &ar, const UINT version)
		{
			ar & itsPItemList;
			ar & itsPItemDTags;
		}

		friend class boost::serialization::access;

	protected:
		PItemList itsPItemList;
		PItemDTagList itsPItemDTags;
	};
}

#endif  //_PBASEWORLD_H
