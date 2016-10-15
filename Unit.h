// (C) 2010 Tim Gurto

#ifndef UNIT_H_H
#define UNIT_H_H

#include "SDL.h"
#include "globals.h"
#include "Entity.h"

struct GameData;
class EntityType;

//TODO comments
class Unit : public Entity{
   typeNum_t player_; //the unit's controlling player

public:
   Unit(typeNum_t type, const Point &loc,
        typeNum_t player = 0);
   const virtual EntityType &Unit::type() const;
   virtual int getColor() const;
   virtual EntityTypeID classID() const;
   virtual bool selectable() const;
};

#endif