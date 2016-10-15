// (C) 2009 Tim Gurto

#ifndef BUILDING_TYPE_H
#define BUILDING_TYPE_H

#include "EntityType.h"

//Entities which are constructed, don't move, and can
//train units
class BuildingType: public EntityType{
   friend class Building;

   //total progress points necessary for construction
   progress_t maxProgress_;

public:
   BuildingType(typeNum_t index,
                const std::string &name,
                const SDL_Rect &drawRect,
                const SDL_Rect &baseRect,
                const Point &selectionCenter,
                progress_t maxProgress = 0);

};

#endif