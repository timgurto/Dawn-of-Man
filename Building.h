// (C) 2009 Tim Gurto

#ifndef BUILDING_H
#define BUILDING_H

#include "globals.h"
#include "Entity.h"

struct GameData;
class EntityType;

//Entities which are constructed, and don't move.
class Building : public Entity{

   progress_t progress_; //construction progress
   bool finished; //whether construction is finished
   float drawPercent; //value for partial drawing

public:
   Building(typeNum_t type, const Point &loc, progress_t progress = 0);
   const virtual EntityType &Building::type() const;
   virtual void tick();
   virtual float getDrawPercent() const;
};


#endif