// (C) 2009 Tim Gurto

#ifndef BUILDING_H
#define BUILDING_H

#include "SDL.h"
#include "globals.h"
#include "Entity.h"
#include "Particle.h"

struct GameData;
class EntityType;

typedef std::list<Particle> particles_t;

//Entities which are constructed, and don't move.
class Building : public Entity{

   progress_t progress_; //construction progress
   bool finished; //whether construction is finished
   float drawPercent; //value for partial drawing

   //particles drawn during construction
   particles_t particles;
   double particlesToDraw;

public:
   Building(typeNum_t type, const Point &loc, progress_t progress = 0);
   const virtual EntityType &Building::type() const;
   virtual void draw(SDL_Surface *screen = screen_) const;
   virtual void drawLater() const;
   virtual void tick();
   virtual float getDrawPercent() const;
};


#endif