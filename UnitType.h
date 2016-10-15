// (C) 2010 Tim Gurto

#ifndef UNIT_TYPE_H
#define UNIT_TYPE_H

#include "globals.h"
#include "EntityType.h"

class UnitType: public EntityType{
   friend class Unit;
   pixels_t speed_; //movement per tick, in px

public:
   UnitType(typeNum_t index,
            const std::string &name,
            const SDL_Rect &drawRect,
            const SDL_Rect &baseRect,
            const Point &selectionCenter,
            double speed = 0);
};

#endif