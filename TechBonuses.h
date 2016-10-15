// (C) 2010 Tim Gurto

#ifndef TECH_BONUSES_H
#define TECH_BONUSES_H

#include "types.h"

//The most hard-coded part of this thing.

struct TechBonuses{
   damage_t unitArmor;

   TechBonuses():
   unitArmor(0){}
};

#endif