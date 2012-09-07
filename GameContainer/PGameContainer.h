//
//
//  @ Project : PorkParser
//  @ File Name : PGameContainer.h
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//

#ifndef _PGAMECONTAINER_H
#define _PGAMECONTAINER_H

#ifndef UINT
#define UINT unsigned int
#endif

#include "PorkString.h"
#include <time.h>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
using std::ifstream;
using std::ofstream;
using std::map;
using std::set;
using std::string;
#include "PMap.h"
#include "PItem.h"

namespace GameContainer
{
	class PGameContainer
	{
	public:
		// Constructors
		PGameContainer() {} // Create an empty PGameContainer
		PGameContainer(string name, string author, float standardVer); // Create a new GameContainer
		~PGameContainer();

		// Basic accessors
		float GetVersion() const { return itsVersion; }
		string GetName() const { return itsName; }
		string GetAuthor() const { return itsAuthor; }
		string GetDateCreated() const { return dateCreated; }

		// Type resolver
		GlobalTypeEnum GetTypeByID(string ID);

		// PMap functions
		void CreatePMap(string ID, string name, string desc);
		PMap* GetPMap(string ID);

		// PItem functions
		void CreatePItem(string ID, string name, string desc);
		PItem* GetPItem(string ID);

	private:
		// Private serialization function for boost::serialization
		template<class Archive>
		void serialize(Archive &ar, const UINT version);

		friend class boost::serialization::access;

	private:
		// GameContainer info
		float itsVersion;
		string itsName;
		string itsAuthor;
		string dateCreated;

		// Game elements
		map<string, PMap> itsPMaps;
		map<string, PItem> itsPItems;
	};
	
	// Related functions that read/write from/to a GameContainer file
	void SaveToFile(const PGameContainer gc);
	PGameContainer ReadFromFile(string filename);
}
#endif  //_PGAMECONTAINER_H