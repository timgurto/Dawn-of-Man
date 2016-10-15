// (C) 2010 Tim Gurto

#ifndef UNIT_H
#define UNIT_H

#include <queue>
#include "SDL.h"
#include "globals.h"
#include "Entity.h"

struct GameData;
class EntityType;

//TODO comments
class Unit : public Entity{
   typeNum_t player_; //the unit's controlling player
   Point target_;
   bool moving_;
   double frameCounter_;
   int frame_;

public:
   Unit(typeNum_t type, const Point &loc,
        typeNum_t player = HUMAN_PLAYER);
   virtual const EntityType &Unit::type() const;
   virtual void draw(SDL_Surface *screen) const;
   virtual void tick(double delta);
   virtual int getColor() const;
   virtual EntityTypeID classID() const;
   virtual bool selectable() const;
   void setTarget(Point target = game_->mousePos -
                                 Point(game_->map));
   bool atTarget();
};

#endif