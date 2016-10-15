// (C) 2009 Tim Gurto

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <vector>
#include <list>

#include "BuildingType.h"
#include "Building.h"

struct GameData{
   std::vector<BuildingType> buildingTypes;
   std::list<Entity *> entities;
};

#endif