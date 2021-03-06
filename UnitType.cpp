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
                   frames_t maxFrameCounter,
                   frames_t frameCount,
                   frames_t maxCombatFrameCounter,
                   frames_t combatFrameCount,
                   frames_t combatWait,
                   damage_t maxHealth,
                   damage_t attack,
                   damage_t armor,
                   bool builder,
                   bool gatherer,
                   bool autoAttack,
                   typeNum_t originBuilding,
                   progress_t maxProgress,
                   typeNum_t resourceAtDeath,
                   typeNum_t prereqTech,
                   typeNum_t decorationAtDeath,
                   const std::string &soundFile,
                   const std::string &deathSoundFile,
                   const std::string &hitSoundFile):
EntityType(index,
           ENT_UNIT,
           name,
           drawRect,
           baseRect,
           selectionCenter,
           decorationAtDeath,
           soundFile,
           deathSoundFile,
           hitSoundFile),
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
autoAttack_(autoAttack),
originBuilding_(originBuilding),
maxProgress_(maxProgress),
resourceAtDeath_(resourceAtDeath),
prereqTech_(prereqTech),
cost_(cost){}

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

typeNum_t UnitType::getPrereqTech() const{
   return prereqTech_;
}

progress_t UnitType::getMaxProgress() const{
   return maxProgress_;
}

damage_t UnitType::getMaxHealth() const{
   return maxHealth_;
}

bool UnitType::isBuilder() const{
   return builder_;
}

bool UnitType::isGatherer() const{
   return gatherer_;
}