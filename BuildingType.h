// (C) 2009 Tim Gurto

#ifndef BUILDING_TYPE_H
#define BUILDING_TYPE_H

#include "EntityType.h"
#include "Resources.h"

//Entities which are constructed, don't move, and can
//train units
class BuildingType: public EntityType{
   friend class Building;
   friend struct CoreData;

   //total progress points necessary for construction
   progress_t maxProgress_;

   //the cost of constructing the building
   Resources cost_;

   //the building's maximum/full health value
   damage_t maxHealth_;

   //how much damage is avoided when attacked
   damage_t armor_;

   //prerequisites
   typeNum_t
      prereqBuilding_,
      prereqTech_;

   //whether the building trains military/expansion units
   bool military_, expansion_;

public:
   BuildingType(typeNum_t index,
                const std::string &name,
                const SDL_Rect &drawRect,
                const SDL_Rect &baseRect,
                const Point &selectionCenter,
                const Resources &cost,
                damage_t maxHealth,
                damage_t armor = 0,
                progress_t maxProgress = 0,
                typeNum_t prereqBuilding = NO_TYPE,
                typeNum_t prereqTech = NO_TYPE,
                typeNum_t decorationAtDeath = NO_TYPE,
                bool military = false,
                bool expansion = false,
                const std::string &soundFile = "",
                const std::string &deathSoundFile = "");

   //get
   const Resources &getCost() const;
   std::string getCostString() const;
   typeNum_t getPrereqTech() const;
   typeNum_t getPrereqBuilding() const;
   progress_t getMaxProgress() const;
   damage_t getMaxHealth() const;
};

#endif