//
//
//  @ Project : PorkParser
//  @ File Name : PItem.h
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#if !defined(_PITEM_H)
#define _PITEM_H

#include "PorkString.h"
using std::string;
#include "PBaseEntity.h"

namespace GameContainer
{
	class PItem : public PBaseEntity
	{
	public:
		// Constructors
		PItem():PBaseEntity("null","null") {}
		PItem(string ID, string name, string desc);
		~PItem();

		// Override basic Accessors of PBaseEntity to prove class is not abstract
		string GetName() const { return PBaseEntity::GetName(); }
		string GetDesc() const { return PBaseEntity::GetDesc(); }

		string GetID() const { return itsID; }
	private:
		template<class Archive>
		void serialize(Archive &ar, const UINT version)
		{
			ar & itsID;
		}

		friend class boost::serialization::access;
	private:
		string itsID;
	};
}

#endif  //_PITEM_H
