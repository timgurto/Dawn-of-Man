// (C) 2010 Tim Gurto

#include "Tech.h"
#include "misc.h"

class Surface;

Tech::Tech(typeNum_t index,
      const std::string &name,
      const std::string &description,
      const TechBonuses &bonuses,
      typeNum_t originBuilding,
      const Resources &cost,
      typeNum_t prereqTech1,
      typeNum_t prereqTech2):
index_(index),
name_(name),
description_(description),
bonuses_(bonuses),
originBuilding_(originBuilding),
cost_(cost),
prereqTech1_(prereqTech1),
prereqTech2_(prereqTech2),
icon_(makePath(ENT_TECH, index, IMG_ICON),
      ENTITY_BACKGROUND){}

Tech::Tech (const Tech &original):
index_(original.index_),
name_(original.name_),
description_(original.description_),
bonuses_(original.bonuses_),
originBuilding_(original.originBuilding_),
cost_(original.cost_),
prereqTech1_(original.prereqTech1_),
prereqTech2_(original.prereqTech2_),
icon_(original.icon_){}

const TechBonuses &Tech::getBonuses() const{
   return bonuses_;
}

typeNum_t Tech::getOriginBuilding() const{
   return originBuilding_;
}

typeNum_t Tech::getIndex() const{
   return index_;
}

const Surface &Tech::getIcon() const{
   return icon_;
}

const Resources &Tech::getCost() const{
   return cost_;
}

std::string Tech::getCostString() const{
   return cost_.str();
}

std::string Tech::getName() const{
   return name_;
}

typeNum_t Tech::getPrereqTech1() const{
   return prereqTech1_;
}

typeNum_t Tech::getPrereqTech2() const{
   return prereqTech2_;
}

std::string Tech::getDescription() const{
   return description_;
}