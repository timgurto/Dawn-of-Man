// (C) 2009-2010 Tim Gurto

#ifndef BUILDING_H
#define BUILDING_H

#include "SDL.h"
#include "types.h"
#include "Entity.h"
#include "Particle.h"

struct GameData;
class EntityType;

//Entities which are constructed, and can't move or attack.
//They train units.
class Building : public Entity{

   //How much progress is made to a building's construction
   //with each builder's hit.
   static const progress_t PROGRESS_PER_BUILDER_HIT;

   progress_t progress_; //construction progress
   bool finished_; //whether construction is finished
   double drawPercent_; //value for partial drawing
   typeNum_t player_; //the building's controlling player

public:
   Building(typeNum_t type, const Point &loc,
            typeNum_t player = HUMAN_PLAYER,
            progress_t progress = 0);

   //Inherited from Entity
   virtual const EntityType &Building::type() const;
   virtual void draw(SDL_Surface *screen = screen_) const;
   virtual EntityTypeID classID() const;
   virtual bool selectable() const;
   virtual int getColor() const;
   virtual typeNum_t getPlayer() const;
   virtual double getDrawPercent() const;

   //progresses the building's construction when a builder
   //hits it
   void progressConstruction();

   //get
   bool isFinished() const;
};

#endif