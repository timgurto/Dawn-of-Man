// (C) 2010 Tim Gurto

#include "GameData.h"
#include "Unit.h"
#include "UnitType.h"

Unit::Unit(typeNum_t type, const Point &loc,
           typeNum_t player):
Entity(type, loc),
player_(player){}

const EntityType &Unit::type() const{
   return game_->unitTypes[type_];
}

int Unit::getColor() const{
   return player_;
}

EntityTypeID Unit::classID() const{
   return UNIT;
}

bool Unit::selectable() const{
   return
      player_ == HUMAN_PLAYER;
}