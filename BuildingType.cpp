// (C) 2009-2010 Tim Gurto

#include "EntityType.h"
#include "BuildingType.h"
#include "types.h"

BuildingType::BuildingType(typeNum_t index,
                           const std::string &name,
                           const SDL_Rect &drawRect,
                           const SDL_Rect &baseRect,
                           const Point &selectionCenter,
                           const Resources &cost,
                           progress_t maxProgress,
                           const std::string &soundFile,
                           const std::string &hitSoundFile,
                           const std::string &deathSoundFile):
EntityType(index,
           ENT_BUILDING,
           name,
           drawRect,
           baseRect,
           selectionCenter,
           soundFile,
           hitSoundFile,
           deathSoundFile),
maxProgress_(maxProgress),
cost_(cost){}

const Resources &BuildingType::getCost() const{
   return cost_;
}

std::string BuildingType::getCostString() const{
   return cost_.str();
}