// (C) 2010 Tim Gurto

#ifndef UNIT_H_H
#define UNIT_H_H

#include "SDL.h"
#include "globals.h"
#include "Entity.h"

struct GameData;
class EntityType;

class Unit : public Entity{

public:
   Unit(typeNum_t type, const Point &loc);
   const virtual EntityType &Unit::type() const;
   virtual EntityTypeID classID() const;
};

#endif