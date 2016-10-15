// (C) 2009 Tim Gurto

#ifndef BUILDING_H
#define BUILDING_H

#include "globals.h"
#include "Entity.h"
#include "EntityType.h"
struct GameData;

class Building : public Entity{
   progress_t progress_;

public:
   Building(typeNum_t type, const Point &loc, progress_t progress = 0);
   const virtual EntityType &Building::type(const GameData &game) const;
};


#endif