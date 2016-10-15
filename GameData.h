// (C) 2009 Tim Gurto

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <vector>
#include <list>

#include "BuildingType.h"
#include "globals.h"

class Building;

typedef std::vector<BuildingType> buildingTypes_t;
typedef std::list<Entity *> entities_t;

struct GameData{
   buildingTypes_t buildingTypes;
   entities_t entities;
   ControlMode mode;
};

#endif