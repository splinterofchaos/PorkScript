//
//
//  @ Project : PorkParser
//  @ File Name : PItemDTag.h
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#if !defined(_PITEMDTAG_H)
#define _PITEMDTAG_H

#ifndef UINT
#define UINT unsigned int
#endif

#include "PorkString.h"
#include <boost/serialization/vector.hpp>
using std::vector;
using std::string;

namespace GameContainer
{
	// struct used to represent a Helper function in a PItemDTag.
	// These functions are used to print dynamic variables at runtime
	// (e.g. the current amount of an item, whether to pluralize its name)
	struct PItemDTagHelper
	{
		string name; // Name of helper function
		vector<string> args; // Arguments for helper
		unsigned int pos; // Position in the main string
		
		PItemDTagHelper()
		{
			pos = 0;
		}
	};

	class PItemDTag
	{
	public:
		// Constructors
		PItemDTag() {};
		PItemDTag(string ID, string desc);
		~PItemDTag();

		// Accessors
		string GetItemID() { return itemID; }
		string GetPartialDesc() { return itsDesc; }
		void AddDescPiece(string piece) { itsDesc += piece; return; }

		void AddDynPiece(PItemDTagHelper dyn) { dynPieces.push_back(dyn); return; }
		vector<PItemDTagHelper> GetDynPieces() { return dynPieces; }
	private:
		template<class Archive>
		void serialize(Archive &ar, const UINT version)
		{
			ar & itemID;
			ar & itsDesc;
			ar & dynPieces;
		}
		friend class boost::serialization::access;
	private:
		string itemID;
		string itsDesc;
		vector<PItemDTagHelper> dynPieces;
	};
}

#endif  //_PITEMDTAG_H