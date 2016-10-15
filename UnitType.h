// (C) 2010 Tim Gurto

#ifndef UNIT_TYPE_H
#define UNIT_TYPE_H

#include "EntityType.h"
#include "types.h"

//TODO comments
class UnitType: public EntityType{
   friend class Unit;
   pixels_t speed_; //movement per tick, in px
   int frameCount_;
   int maxFrameCounter_;
   int combatFrameCount_;
   int maxCombatFrameCounter_;
   int combatWait_;
   int maxHealth_;
   int attack_;
   int armor_;

public:
   UnitType(typeNum_t index,
            const std::string &name,
            const SDL_Rect &drawRect,
            const SDL_Rect &baseRect,
            const Point &selectionCenter,
            pixels_t speed = 0,
            int maxFrameCounter = 0,
            int frameCount = 1,
            int maxCombatFrameCounter = 0,
            int combatFrameCount = 1,
            int combatWait = 1,
            damage_t maxHealth = 1,
            damage_t attack = 0,
            damage_t armor = 0);
};

#endif