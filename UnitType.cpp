// (C) 2010 Tim Gurto


#include "EntityType.h"
#include "UnitType.h"
#include "types.h"

UnitType::UnitType(typeNum_t index,
                   const std::string &name,
                   const SDL_Rect &drawRect,
                   const SDL_Rect &baseRect,
                   const Point &selectionCenter,
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
                   typeNum_t originBuilding,
                   progress_t maxProgress):
EntityType(index,
           ENT_UNIT,
           name,
           drawRect,
           baseRect,
           selectionCenter),
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
originBuilding_(originBuilding),
maxProgress_(maxProgress){}

typeNum_t UnitType::getOriginBuilding() const{
   return originBuilding_;
}

bool UnitType::isBuilder() const{
   return builder_;
}