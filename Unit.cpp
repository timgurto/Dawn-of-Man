// (C) 2010 Tim Gurto

#include "GameData.h"
#include "Unit.h"
#include "UnitType.h"

Unit::Unit(typeNum_t type, const Point &loc):
Entity(type, loc){}

const EntityType &Unit::type() const{
   return game_->unitTypes[type_];
}

EntityTypeID Unit::classID() const{
   return UNIT;
}