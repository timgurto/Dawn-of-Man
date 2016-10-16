// (C) 2010 Tim Gurto

#ifndef CORE_DATA_H
#define CORE_DATA_H

#include <string>
#include <vector>

#include "BuildingType.h"
#include "DecorationType.h"
#include "UnitType.h"
#include "ResourceNodeType.h"
#include "Tech.h"

typedef std::vector<BuildingType> buildingTypes_t;
typedef std::vector<DecorationType> decorationTypes_t;
typedef std::vector<UnitType> unitTypes_t;
typedef std::vector<ResourceNodeType> resourceNodeTypes_t;
typedef std::vector<Tech> techs_t;

//Resource names, entity types, techs... everything
//static that is loaded from data files.
struct CoreData{
   
   //Entity types
   //vectors: contiguous for index access
   buildingTypes_t buildingTypes;
   decorationTypes_t decorationTypes;
   unitTypes_t unitTypes;
   resourceNodeTypes_t resourceNodeTypes;
   techs_t techs;

   CoreData(std::string fileName);
};

#endif