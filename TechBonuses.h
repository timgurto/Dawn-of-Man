// (C) 2010 Tim Gurto

#ifndef TECH_BONUSES_H
#define TECH_BONUSES_H

#include "types.h"

//The most hard-coded part of this thing.

struct TechBonuses{
   damage_t unitArmor;

   TechBonuses(damage_t unitArmorArg = 0);

   TechBonuses &operator+=(const TechBonuses rhs);
};

#endif