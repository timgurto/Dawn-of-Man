// (C) 2009 Tim Gurto

#include "Building.h"
#include "GameData.h"
#include "Debug.h"

extern Debug debug;

Building::Building(typeNum_t type, const Point &loc, progress_t progress):
Entity(type, loc),
progress_(progress),
finished(false),
alpha(TRANSPARENT){}

const EntityType &Building::type() const{
   return game_->buildingTypes[type_];
}

void Building::tick(){
   if (!finished){
      progress_ += PROGRESS_PER_CALC;
      //debug("progress = ", progress_);
      if (progress_ >= game_->buildingTypes[type_].progress_){
         finished = true;
         alpha = OPAQUE;
         debug("building finished");
      }else
         alpha = 255 * progress_ /
                 game_->buildingTypes[type_].progress_;
   }
}

Uint8 Building::getAlpha() const{
   return alpha;
}