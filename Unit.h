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
   Point target;
   Unit(typeNum_t type, const Point &loc,
        typeNum_t player = HUMAN_PLAYER);
   const virtual EntityType &Unit::type() const;
   virtual void tick(double delta);
   virtual int getColor() const;
   virtual EntityTypeID classID() const;
   virtual bool selectable() const;
};

#endif