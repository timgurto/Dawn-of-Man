// (C) 2009-2010 Tim Gurto

#include "GameData.h"
#include "Decoration.h"
#include "DecorationType.h"

Decoration::Decoration(typeNum_t type, const Point &loc):
Entity(type, loc){}

const EntityType &Decoration::type() const{
   return game_->decorationTypes[typeIndex_];
}

EntityTypeID Decoration::classID() const{
   return DECORATION;
}

bool Decoration::collides() const{
   return false;
}