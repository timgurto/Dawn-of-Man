// (C) 2009, 2010 Tim Gurto

#ifndef BUILDING_H
#define BUILDING_H

#include "types.h"
#include "Entity.h"
#include "Particle.h"

struct GameData;
class EntityType;

//Entities which are constructed, and can't move or attack.
//They train units.
class Building : public Entity{
   friend struct GameData;
   friend class Entity;

   //How much progress is made to a building's construction
   //with each builder's hit.
   static const progress_t PROGRESS_PER_BUILDER_HIT;

   damage_t health_; //the building's remaining health
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
   virtual void draw(Surface &screen = screenBuf) const;
   virtual EntityTypeID classID() const;
   virtual bool selectable() const;
   virtual bool targetable() const;
   virtual bool drawBlack() const;

   //progresses the building's construction when a builder
   //hits it
   void progressConstruction();

   //removes health, due to an attack
   void removeHealth(damage_t damage);

   //get
   bool isFinished() const;
   virtual int getColor() const;
   virtual typeNum_t getPlayer() const;
   virtual double getDrawPercent() const;
   virtual std::string getHelp() const;
   damage_t getArmor() const;
   damage_t getHealth() const;
   bool isMilitary() const;
   bool isExpansion() const;
};

#endif