//
//
//  @ Project : PorkParser
//  @ File Name : PGameContainer.cpp
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//

#pragma warning(push)
#pragma warning(disable : 4996)

#include "PGameContainer.h"

namespace GameContainer
{
	void SaveToFile(const PGameContainer gc)
	{
		// Create filestream
		ofstream ofs(gc.GetName()+string(".pgf"));
		// Save all data to binary archive
		boost::archive::text_oarchive oa(ofs);
		oa << gc;
	}

	PGameContainer ReadFromFile(string filename)
	{
		// Create filestream
		ifstream ifs(filename);
		// Read in serialized data
		boost::archive::text_iarchive ia(ifs);
		PGameContainer tempGC;
		ia >> tempGC;

		return tempGC;
	}

	// Constructor for new GameContainer
	PGameContainer::PGameContainer(string name, string author, float standardVer):
	itsName(name), itsAuthor(author), itsVersion(standardVer)
	{
		// Get the time, convert it to GMT, and use it as the dateCreated
		time_t locTime = time(NULL);
		struct tm *ptm;

		ptm = gmtime(&locTime);

		dateCreated = asctime(ptm);
	}
	
	// Destructor
	PGameContainer::~PGameContainer()
	{
	}
	
	void PGameContainer::CreatePMap(string ID, string name, string desc)
	{
		if(itsPMaps.count(ID) == 0)
		{
			PMap newMap(ID, name, desc);
			itsPMaps.insert(std::pair<string, PMap>(ID, newMap));
		}
		return;
	}
	
	GlobalTypeEnum PGameContainer::GetTypeByID(string ID)
	{
		if(itsPMaps.count(ID) >= 1)
			return MAP;
		else if(itsPItems.count(ID) >= 1)
			return ITEM;
		else
			return NONETYPE;
	}

	PMap* PGameContainer::GetPMap(string ID)
	{
		if(itsPMaps.count(ID) >= 1)
			return &itsPMaps[ID];
		else
			return NULL;
	}
	
	void PGameContainer::CreatePItem(string ID, string name, string desc)
	{
		
		if(itsPItems.count(ID) == 0)
		{
			PItem newItem(ID, name, desc);
			itsPItems.insert(std::pair<string, PItem>(ID, newItem));
		}

		return;
	}
	
	PItem* PGameContainer::GetPItem(string ID)
	{
		if(itsPItems.count(ID) >= 1)
			return &itsPItems[ID];
		else
			return NULL;
	}
	
	template<class Archive>
	void PGameContainer::serialize(Archive &ar, const UINT version)
	{
		// Serialize basic members
		ar & itsVersion;
		ar & itsName;
		ar & itsAuthor;
		ar & dateCreated;

		// Serialize maps
		ar & itsPMaps;
		ar & itsPItems;
	}
}

#pragma warning(pop)