// (C) 2009 Tim Gurto

#ifndef BUILDING_H
#define BUILDING_H

#include "SDL.h"
#include "globals.h"
#include "Entity.h"

struct GameData;
class EntityType;

//Entities which are constructed, and don't move.
class Decoration : public Entity{

public:
   Decoration(typeNum_t type, const Point &loc);
   const virtual EntityType &Decoration::type() const;
   virtual EntityTypeID classID() = 0;
};


#endif