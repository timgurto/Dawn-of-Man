// (C) 2009 Tim Gurto

#include "GameData.h"
#include "Decoration.h"
#include "DecorationType.h"

Decoration::Decoration(typeNum_t type, const Point &loc):
Entity(type, loc){}

const EntityType &Decoration::type() const{
   return game_->decorationTypes[type_];
}

int Decoration::getColor() const{
   return ENTITY_DECORATION;
}

EntityTypeID Decoration::classID() const{
   return DECORATION;
}