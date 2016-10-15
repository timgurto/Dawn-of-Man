// (C) 2010 Tim Gurto

#ifndef UNIT_TYPE_H
#define UNIT_TYPE_H

#include "EntityType.h"
#include "types.h"

//Entities which can move and engage in combat.
//They are trained at buildings.
class UnitType: public EntityType{
   friend class Unit;

   pixels_t speed_; //movement per tick, in px
   
   //the number of walking animation frames
   int frameCount_;
   
   //how long the walking animation cycle takes
   int maxFrameCounter_;

   //the number of combat animation frames
   int combatFrameCount_;

   //how long the combat anumation cycle takes
   int maxCombatFrameCounter_;

   //extra wait time before each combat cycle
   int combatWait_;

   //the unit's maximum/full health value
   damage_t maxHealth_;

   //how much damage the unit deals in combat
   damage_t attack_;

   //how much damage is avoided when attacked
   damage_t armor_;

   //the building where this unit can be trained.
   //NO_TYPE indicates that the unit can't be trained
   typeNum_t originBuilding_;

   //total progress points necessary for construction
   progress_t maxProgress_;

   //whether this unit can construct buildings
   bool builder_;


public:

   UnitType(typeNum_t index,
            const std::string &name,
            const SDL_Rect &drawRect,
            const SDL_Rect &baseRect,
            const Point &selectionCenter,
            pixels_t speed = 0,
            int maxFrameCounter = 0,
            int frameCount = 1,
            int maxCombatFrameCounter = 0,
            int combatFrameCount = 1,
            int combatWait = 1,
            damage_t maxHealth = 1,
            damage_t attack = 0,
            damage_t armor = 0,
            bool builder = false,
            typeNum_t originBuilding = NO_TYPE,
            progress_t maxProgress = 0);

   //get
   typeNum_t getOriginBuilding() const;
   bool getBuilder() const;
};

#endif