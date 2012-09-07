//
//
//  @ Project : PorkParser
//  @ File Name : PBaseEntity.h
//  @ Date : 9/16/2011
//  @ Author : Andy Brennan
//
//


#if !defined(_PBASEENTITY_H)
#define _PBASEENTITY_H

#ifndef UINT
#define UINT unsigned int
#endif

#include "PorkString.h"
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
using std::vector;
using std::map;
using std::string;
using std::pair;
#include "GlobalTypeEnum.h"
#include "GlobalExceptions.h"

namespace GameContainer
{
	typedef std::pair<string, string> PropQualifier; // ReqFile.prop qualifier name for properties
	typedef std::pair<GlobalTypeEnum, string> PropValue;

	class PBaseEntity
	{
	public:
		// Constructors
		PBaseEntity(string name, string desc);
		virtual ~PBaseEntity();

		// Basic Accessors
		virtual string GetName() const = 0 { return itsName; }
		virtual string GetDesc() const = 0 { return itsDesc; }

		// Property Accessors
		void SetCustomProp(string name, string value, string reqFileName, GlobalTypeEnum type);

		string GetPropAsString(string name, string reqFileName);
		long   GetPropAsLong(string name, string reqFileName);
		bool   GetPropAsBool(string name, string reqFileName);
		
		bool PropertyExist(string module, string name);
		GlobalTypeEnum GetPropType(string module, string name);

		// Dynamic property list accessors
		void SetDynProp(string masterProp, string reqFileName, GlobalTypeEnum identType, string dynIdentifier, string dynValue);

		string GetDynPropAsString(string masterProp, string reqFileName, GlobalTypeEnum identType, string dynIdentifier);
		long GetDynPropAsLong(string masterProp, string reqFileName, GlobalTypeEnum identType, string dynIdentifier);
		bool GetDynPropAsBool(string masterProp, string reqFileName, GlobalTypeEnum identType, string dynIdentifier);

		// Overall Property Funcs
		void SetPropList(map<PropQualifier, PropValue> list) { propList = list; }
		void SetDynVersions(map<PropQualifier, map<PropValue,PropValue>> dyns) { dynVersions = dyns; }

	private:
		// Private methods/members for boost::serialization
		template<class Archive> 
		void serialize(Archive &ar, const UINT version)
		{
			// Basic members
			ar & itsName;
			ar & itsDesc;

			// Properties
			ar & propList;
			ar & dynVersions;
		}

		friend class boost::serialization::access;
	protected:
		string itsName;
		string itsDesc;

		// Custom properties container
		map<PropQualifier, PropValue> propList;
		map<PropQualifier, map<PropValue,PropValue>> dynVersions;
	};
}

#endif  //_PBASEENTITY_H