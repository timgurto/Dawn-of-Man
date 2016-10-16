// (C) 2009, 2010 Tim Gurto

#include "EntityType.h"
#include "BuildingType.h"
#include "types.h"

BuildingType::BuildingType(typeNum_t index,
                           const std::string &name,
                           const SDL_Rect &drawRect,
                           const SDL_Rect &baseRect,
                           const Point &selectionCenter,
                           const Resources &cost,
                           damage_t maxHealth,
                           damage_t armor,
                           progress_t maxProgress,
                           typeNum_t prereqBuilding,
                           typeNum_t prereqTech,
                           typeNum_t decorationAtDeath,
                           const std::string &soundFile,
                           const std::string &deathSoundFile):
EntityType(index,
           ENT_BUILDING,
           name,
           drawRect,
           baseRect,
           selectionCenter,
           decorationAtDeath,
           soundFile,
           deathSoundFile),
maxProgress_(maxProgress),
maxHealth_(maxHealth),
armor_(armor),
cost_(cost),
prereqBuilding_(prereqBuilding),
prereqTech_(prereqTech){}

const Resources &BuildingType::getCost() const{
   return cost_;
}

std::string BuildingType::getCostString() const{
   return cost_.str();
}

typeNum_t BuildingType::getPrereqTech() const{
   return prereqTech_;
}

typeNum_t BuildingType::getPrereqBuilding() const{
   return prereqBuilding_;
}

progress_t BuildingType::getMaxProgress() const{
   return maxProgress_;
}

damage_t BuildingType::getMaxHealth() const{
   return maxHealth_;
}