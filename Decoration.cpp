// (C) 2009 Tim Gurto

#include "Decoration.h"
#include "GameData.h"
#include "DecorationType.h"

Decoration::Decoration(typeNum_t type, const Point &loc):
Entity(type, loc){}

const EntityType &Decoration::type() const{
   return game_->decorationTypes[type_];
}