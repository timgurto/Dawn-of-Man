// (C) 2010 Tim Gurto

#include "TechBonuses.h"

TechBonuses::TechBonuses(damage_t unitArmorArg):
unitArmor(unitArmorArg){}

TechBonuses &TechBonuses::operator+=(const TechBonuses rhs){
   unitArmor += rhs.unitArmor;
   return *this;
}