// (C) 2009 Tim Gurto

#include "Building.h"
#include "GameData.h"

Building::Building(typeNum_t type, const Point &loc, progress_t progress):
Entity(type, loc),
progress_(progress){}

const EntityType &Building::type(const GameData &game) const{
   return game.buildingTypes[type_];
}