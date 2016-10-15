// (C) 2009 Tim Gurto

#include "EntityType.h"
#include "BuildingType.h"
#include "globals.h"

BuildingType::BuildingType(typeNum_t index,
                           const std::string &name,
                           const SDL_Rect &drawRect,
                           const SDL_Rect &baseRect,
                           const Point &selectionCenter,
                           progress_t progress):
EntityType(index,
           BUILDING,
           name,
           drawRect,
           baseRect,
           selectionCenter),
progress_(progress){}