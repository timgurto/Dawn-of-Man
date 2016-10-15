// (C) 2009 Tim Gurto

#ifndef BUILDING_TYPE_H
#define BUILDING_TYPE_H

#include "EntityType.h"

class BuildingType: public EntityType{
   progress_t progress_;

public:
   BuildingType(typeNum_t index,
                const std::string &name,
                const SDL_Rect &drawRect,
                const SDL_Rect &baseRect,
                const SDL_Color &background,
                progress_t progress = 0);
   //BuildingType(BuildingType &original);
   //BuildingType &operator=(const BuildingType &rhs);

};

#endif