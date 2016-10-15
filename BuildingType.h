// (C) 2009 Tim Gurto

#ifndef BUILDING_TYPE_H
#define BUILDING_TYPE_H

#include "EntityType.h"
#include "Resources.h"

//Entities which are constructed, don't move, and can
//train units
class BuildingType: public EntityType{
   friend class Building;

   //total progress points necessary for construction
   progress_t maxProgress_;

   //the cost of constructing the building
   Resources cost_;

public:
   BuildingType(typeNum_t index,
                const std::string &name,
                const SDL_Rect &drawRect,
                const SDL_Rect &baseRect,
                const Point &selectionCenter,
                const Resources &cost,
                progress_t maxProgress = 0);

   //get
   const Resources &getCost();
   std::string getCostString() const;
};

#endif