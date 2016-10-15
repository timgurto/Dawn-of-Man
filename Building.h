// (C) 2009 Tim Gurto

#ifndef BUILDING_H
#define BUILDING_H

#include "globals.h"
#include "Entity.h"

struct GameData;
class EntityType;

class Building : public Entity{

   progress_t progress_;
   bool finished;
   Uint8 alpha;

public:
   Building(typeNum_t type, const Point &loc, progress_t progress = 0);
   const virtual EntityType &Building::type() const;
   virtual void tick();
   virtual Uint8 getAlpha() const;
};


#endif