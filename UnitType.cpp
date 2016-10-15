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
                   int combatWait):
EntityType(index,
           UNIT,
           name,
           drawRect,
           baseRect,
           selectionCenter),
speed_(speed),
maxFrameCounter_(maxFrameCounter),
frameCount_(frameCount),
maxCombatFrameCounter_(maxCombatFrameCounter),
combatFrameCount_(combatFrameCount),
combatWait_(combatWait){}