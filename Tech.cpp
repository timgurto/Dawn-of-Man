// (C) 2010 Tim Gurto

#include "Tech.h"
#include "misc.h"

Tech::Tech(typeNum_t index,
      const std::string &name,
      const TechBonuses &bonuses,
      typeNum_t originBuilding,
      const Resources &cost,
      typeNum_t prereqTech):
index_(index),
name_(name),
bonuses_(bonuses),
originBuilding_(originBuilding),
cost_(cost),
prereqTech_(prereqTech){
   icon_ = loadImage(makePath(ENT_TECH,
                             index_,
                             IMG_ICON),
                    ENTITY_BACKGROUND);
}

Tech::Tech (const Tech &original):
index_(original.index_),
name_(original.name_),
bonuses_(original.bonuses_),
originBuilding_(original.originBuilding_),
cost_(original.cost_),
prereqTech_(original.prereqTech_),
icon_(copySurface(original.icon_)){}

Tech::~Tech(){
   freeSurface(icon_);
}

const TechBonuses &Tech::getBonuses() const{
   return bonuses_;
}

typeNum_t Tech::getOriginBuilding() const{
   return originBuilding_;
}

typeNum_t Tech::getIndex() const{
   return index_;
}

SDL_Surface *Tech::getIcon() const{
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