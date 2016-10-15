// (C) 2009-2010 Tim Gurto

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
   bool finished_; //whether construction is finished
   double drawPercent_; //value for partial drawing
   typeNum_t player_; //the building's controlling player

public:
   Building(typeNum_t type, const Point &loc,
            typeNum_t player = HUMAN_PLAYER,
            progress_t progress = 0);
   const virtual EntityType &Building::type() const;
   virtual void draw(SDL_Surface *screen = screen_) const;
   virtual void tick(double delta);
   virtual double getDrawPercent() const;
   virtual int getColor() const;
   virtual EntityTypeID classID() const;
   virtual bool selectable() const;
};

#endif