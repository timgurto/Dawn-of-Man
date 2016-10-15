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
   progress_t progress_; //training progress
   bool finished_; //whether the unit is fully trained
   double drawPercent_; //value for partial drawing
   
   //the current points in the unit's animation cycles
   double frameCounter_;
   double combatFrameCounter_;

   int frame_; //which frame to draw for this unit

public:
   Unit(typeNum_t type, const Point &loc,
        typeNum_t player = HUMAN_PLAYER,
        progress_t progress = 0);

   //Inherited from Entity
   virtual const EntityType &Unit::type() const;
   virtual void draw(SDL_Surface *screen) const;
   virtual void tick(double delta);
   virtual EntityTypeID classID() const;
   virtual bool selectable() const;
   virtual bool targetable() const;

   //sets the target entity, and co-ordinates.  If an
   //entity isn't provided, use the specified co-ordinates
   void setTarget(Entity *targetEntity = 0,
                  Point loc = game_->mousePos -
                              Point(game_->map));

   //Whether the unit has reached its target
   bool atTarget() const;

   //sets target co-ordinates if targetting an entity
   void updateTarget();

   //get
   bool isBuilder() const;
   Entity *getTargetEntity() const;
   virtual int getColor() const;
   virtual typeNum_t getPlayer() const;
   virtual double getDrawPercent() const;
};

#endif