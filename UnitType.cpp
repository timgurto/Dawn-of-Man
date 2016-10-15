// (C) 2010 Tim Gurto


#include "EntityType.h"
#include "UnitType.h"
#include "Resources.h"
#include "types.h"

UnitType::UnitType(typeNum_t index,
                   const std::string &name,
                   const SDL_Rect &drawRect,
                   const SDL_Rect &baseRect,
                   const Point &selectionCenter,
                   const Resources &cost,
                   pixels_t speed,
                   int maxFrameCounter,
                   int frameCount,
                   int maxCombatFrameCounter,
                   int combatFrameCount,
                   int combatWait,
                   damage_t maxHealth,
                   damage_t attack,
                   damage_t armor,
                   bool builder,
                   bool gatherer,
                   typeNum_t originBuilding,
                   progress_t maxProgress,
                   typeNum_t resourceAtDeath,
                   const std::string &soundFile,
                   const std::string &hitSoundFile,
                   const std::string &deathSoundFile):
EntityType(index,
           ENT_UNIT,
           name,
           drawRect,
           baseRect,
           selectionCenter,
           soundFile,
           hitSoundFile,
           deathSoundFile),
speed_(speed),
maxFrameCounter_(maxFrameCounter),
frameCount_(frameCount),
maxCombatFrameCounter_(maxCombatFrameCounter),
combatFrameCount_(combatFrameCount),
combatWait_(combatWait),
maxHealth_(maxHealth),
attack_(attack),
armor_(armor),
builder_(builder),
gatherer_(gatherer),
originBuilding_(originBuilding),
maxProgress_(maxProgress),
cost_(cost),
resourceAtDeath_(resourceAtDeath){}

typeNum_t UnitType::getOriginBuilding() const{
   return originBuilding_;
}

const Resources &UnitType::getCost() const{
   return cost_;
}

std::string UnitType::getCostString() const{
   return cost_.str();
}

typeNum_t UnitType::getDeathResource() const{
   return resourceAtDeath_;
}