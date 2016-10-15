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

//Contains all data necessary to describe the game's
//state.  Makes passing this information to classes
//and functions much simpler.
struct GameData{
   
   //Types of Buildings
   buildingTypes_t buildingTypes;
   
   //All entities in the game, sorted by y co-ordinate.
   //Entity* used for polymorphism
   entities_t entities;

   //The current game mode
   ControlMode mode;
};

#endif