// (C) 2010 Tim Gurto

#ifndef UNIT_H
#define UNIT_H

#include <queue>
#include "SDL.h"
#include "types.h"
#include "Entity.h"
#include "Point.h"

struct GameData;
class EntityType;

//Mobile, controllable entities capable of combat.
//Created at buildings
class Unit : public Entity{
   friend class Entity;

   typeNum_t player_; //the unit's controlling player
   Point target_; //where the unit is trying to go
   Entity *targetEntity_; //the unit this unit is targetting
   bool moving_; //whether or not the unit is moving
   bool combat_; //whether or not the unit is attacking another
   damage_t health_; //the unit's remaining health
   progress_t progress_;
   bool finished_;
   double drawPercent_;
   
   //the current point in the unit's animation cycle
   double frameCounter_;
   double combatFrameCounter_;
   int frame_; //which frame to draw for this unit

public:
   Unit(typeNum_t type, const Point &loc,
        typeNum_t player = HUMAN_PLAYER,
        progress_t progress = 0);
   virtual const EntityType &Unit::type() const;
   virtual void draw(SDL_Surface *screen) const;
   virtual void tick(double delta);
   virtual int getColor() const;
   virtual EntityTypeID classID() const;
   virtual bool selectable() const;
   virtual typeNum_t getPlayer() const;

   //TODO add loc default to GameData, updated with mouse move
   //sets the target co-ordinates
   void setTarget(Entity *targetEntity,
                  Point loc = game_->mousePos -
                              Point(game_->map));

   //Whether or not the unit has reached its target
   bool atTarget();

   //sets target co-ordinates if targetting an entity
   void updateTarget();
};

#endif