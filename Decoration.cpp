// (C) 2009-2010 Tim Gurto

#include "Decoration.h"
#include "DecorationType.h"
#include "CoreData.h"

Decoration::Decoration(typeNum_t type, const Point &loc):
Entity(type, loc){}

const EntityType &Decoration::type() const{
   return core_->decorationTypes[typeIndex_];
}

EntityTypeID Decoration::classID() const{
   return ENT_DECORATION;
}

bool Decoration::collides() const{
   return core_->decorationTypes[typeIndex_].collides_;
}

int Decoration::getColor() const{
   return core_->decorationTypes[typeIndex_].color_;
}