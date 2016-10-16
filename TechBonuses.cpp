// (C) 2010 Tim Gurto

#include <algorithm>
#include "TechBonuses.h"

TechBonuses::TechBonuses(damage_t unitHealthArg,
                         damage_t unitArmorArg,
                         damage_t attackArg,
                         damage_t buildingHealthArg,
                         damage_t buildingArmorArg,
                         pixels_t speedArg,
                         progress_t trainingSpeedArg,
                         progress_t buildingSpeedArg,
                         const resources_t &gatheringArg):
unitHealth(unitHealthArg),
unitArmor(unitArmorArg),
attack(attackArg),
buildingHealth(buildingHealthArg),
buildingArmor(buildingArmorArg),
speed(speedArg),
trainingSpeed(trainingSpeedArg),
buildingSpeed(buildingSpeedArg),
gathering(gatheringArg){
   while (gathering.size() < Resources::getResourceCount())
      gathering.push_back(0);
}

TechBonuses &TechBonuses::operator+=(const TechBonuses rhs){
   unitHealth += rhs.unitHealth;
   unitArmor += rhs.unitArmor;
   attack += rhs.attack;
   buildingHealth += rhs.buildingHealth;
   buildingArmor += rhs.buildingArmor;
   speed += rhs.speed;
   trainingSpeed += rhs.trainingSpeed;
   buildingSpeed += rhs.buildingSpeed;
   for (size_t i = 0; i != rhs.gathering.size(); ++i){
      gathering[i] += rhs.gathering[i];
   }
   return *this;
}