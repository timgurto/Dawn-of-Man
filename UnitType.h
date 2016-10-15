// (C) 2010 Tim Gurto

#ifndef UNIT_TYPE_H_H
#define UNIT_TYPE_H_H

#include "EntityType.h"

class UnitType: public EntityType{
   friend class Unit;


public:
   UnitType(typeNum_t index,
            const std::string &name,
            const SDL_Rect &drawRect,
            const SDL_Rect &baseRect,
            const Point &selectionCenter);
};

#endif