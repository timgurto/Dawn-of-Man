// (C) 2010 Tim Gurto

#ifndef TECH_BONUSES_H
#define TECH_BONUSES_H

#include "types.h"
#include "Resources.h"

struct TechBonuses{
   damage_t
      unitHealth,
      unitArmor,
      attack,
      buildingHealth,
      buildingArmor;
   pixels_t
      speed;
   progress_t
      trainingSpeed,
      buildingSpeed;
   resources_t
      gathering;

   TechBonuses(damage_t unitHealthArg = 0,
               damage_t unitArmorArg = 0,
               damage_t attackArg = 0,
               damage_t buildingHealthArg = 0,
               damage_t buildingArmorArg = 0,
               pixels_t speedArg = 0,
               progress_t trainingSpeed = 0,
               progress_t buildingSpeed = 0,
               const resources_t &gatheringArg = resources_t());

   TechBonuses &operator+=(const TechBonuses rhs);
};

#endif