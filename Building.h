// (C) 2009 Tim Gurto

#ifndef BUILDING_H
#define BUILDING_H

#include "SDL.h"
#include "globals.h"
#include "Entity.h"
#include "Particle.h"

struct GameData;
class EntityType;

//Entities which are constructed, and don't move.
class Building : public Entity{

   progress_t progress_; //construction progress
   bool finished; //whether construction is finished
   float drawPercent; //value for partial drawing
   typeNum_t player_; //the building's controlling player

public:
   Building(typeNum_t type, const Point &loc,
            typeNum_t player = 0,
            progress_t progress = 0);
   const virtual EntityType &Building::type() const;
   virtual void draw(SDL_Surface *screen = screen_) const;
   virtual void tick();
   virtual float getDrawPercent() const;
   virtual Uint32 getColor() const;
};


#endif