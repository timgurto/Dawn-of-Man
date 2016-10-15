// (C) 2009 Tim Gurto

#include "Building.h"
#include "GameData.h"
#include "Debug.h"

extern Debug debug;

Building::Building(typeNum_t type, const Point &loc, progress_t progress):
Entity(type, loc),
progress_(progress),
finished(false),
drawPercent(EMPTY){}

const EntityType &Building::type() const{
   return game_->buildingTypes[type_];
}

void Building::tick(){
   if (!finished){
      progress_ += PROGRESS_PER_CALC;
      //debug("progress = ", progress_);
      if (progress_ >= game_->buildingTypes[type_].progress_){
         finished = true;
         drawPercent = FULL;
         debug("building finished");
      }else
         drawPercent = 1.0f * progress_ /
                 game_->buildingTypes[type_].progress_;
   }
}

float Building::getDrawPercent() const{
   return drawPercent;
}