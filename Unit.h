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
   friend class Building;
   friend struct GameData;

   typeNum_t player_; //the unit's controlling player
   bool moving_; //whether or not the unit is moving
   bool combat_; //whether or not the unit is attacking another
   damage_t health_; //the unit's remaining health
   progress_t progress_; //training progress
   bool finished_; //whether the unit is fully trained
   double drawPercent_; //value for partial drawing

   //target/pathing
   Point target_; //where the unit is trying to go
   Entity *targetEntity_; //the unit this unit is targetting
   path_t path_; //the path to the target
   
   //the current points in the unit's animation cycles
   double frameCounter_;
   double combatFrameCounter_;
   int frame_; //which frame to draw for this unit

   //smaller = finer, more accurate, slower pathfinding
   static const pixels_t PATH_GRID_SIZE;

   //the distance at which units will automatically attack enemies
   static const pixels_t AUTO_ATTACK_DISTANCE;

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
   virtual bool drawBlack() const;

   //sets the target entity, and co-ordinates.  If an
   //entity isn't provided, use the specified co-ordinates
   void setTarget(Entity *targetEntity = 0,
                  Point loc = game_->mousePos -
                              Point(game_->map));

   //Whether the unit has reached its target
   bool isAtTarget() const;

   //sets target co-ordinates if targetting an entity
   void updateTarget();

   //Whether the unit would have a clear path between
   //the two specified points
   bool isPathClear(const Point &start,
                    const Point &end) const;

   //calculates a path to the target, and fills the path_
   //with 
   void findPath(pixels_t gridSize = PATH_GRID_SIZE);

   //removes health, due to an attack
   void removeHealth(damage_t damage);

   Entity *findNearbyEnemy();

   //get
   bool isBuilder() const;
   bool isGatherer() const;
   Entity *getTargetEntity() const;
   damage_t getArmor() const;
   virtual int getColor() const;
   virtual typeNum_t getPlayer() const;
   virtual double getDrawPercent() const;
   virtual std::string getHelp() const;
   damage_t getHealth() const;
   damage_t getAttack() const;
   pixels_t getSpeed() const;
};

#endif