// (C) 2009-2010 Tim Gurto

#include <list>
#include <cassert>
#include "SDL.h"
#include "Building.h"
#include "GameData.h"
#include "Debug.h"
#include "Particle.h"
#include "BuildingType.h"

extern Debug debug;

Building::Building(typeNum_t type, const Point &loc,
                   typeNum_t player, progress_t progress):
Entity(type, loc),
progress_(progress),
player_(player),
finished_(progress >= game_->buildingTypes[type].maxProgress_),
drawPercent_(game_->buildingTypes[type].maxProgress_ == 0 ?
                1.0 :
                1.0f * progress_ /
                game_->buildingTypes[type].maxProgress_){}

const EntityType &Building::type() const{
   return game_->buildingTypes[typeIndex_];
}

void Building::draw(SDL_Surface *screen) const{
   const EntityType &thisType = type();
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;

   pixels_t
      partialW = pixels_t(drawPercent_ *
                          thisType.drawRect_.w),
      partialH = pixels_t(drawPercent_ *
                          thisType.drawRect_.h);

   //clip, based on randomized direction
   SDL_Rect srcLoc = getSrcClip(partialW, partialH);

   switch(direction_){
   case DIR_LEFT:
      drawLoc.x += thisType.drawRect_.w - partialW;
      break;
   case DIR_UP:
      drawLoc.y += thisType.drawRect_.h - partialH;
   }

   colorBlit(getColor(), screen, srcLoc, drawLoc);
}

void Building::tick(double delta){
   if (!finished_){
      progress_ += delta * PROGRESS_PER_CALC;
      //debug("progress = ", progress_);
      if (progress_ >= game_->buildingTypes[typeIndex_].maxProgress_){
         finished_ = true;
         drawPercent_ = FULL;
         //debug("building finished");
      }else
         drawPercent_ = 1.0 * progress_ /
                 game_->buildingTypes[typeIndex_].maxProgress_;

      int particlesToDraw = int(1.0 * rand() / RAND_MAX +
                                0.02 * 
                                Particle::PARTICLE_COUNT *
                                delta * Particle::DECAY *
                                1);//delta);

      addParticles(particlesToDraw);
   }
}

double Building::getDrawPercent() const{
   return drawPercent_;
}

int Building::getColor() const{
   return player_;
}

EntityTypeID Building::classID() const{
   return ENT_BUILDING;
}

bool Building::selectable() const{
   return
      finished_ &&
      player_ == HUMAN_PLAYER;
}

typeNum_t Building::getPlayer() const{
   return player_;
}