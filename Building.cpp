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
drawPercent_(1.0f *
            progress_ /
            game_->buildingTypes[type].maxProgress_){}

const EntityType &Building::type() const{
   return game_->buildingTypes[typeIndex_];
}

void Building::draw(SDL_Surface *screen) const{
   const EntityType &thisType = type();
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;

   SDL_Rect srcLoc;
   pixels_t
      partialW = pixels_t(getDrawPercent() *
                          thisType.drawRect_.w),
      partialH = pixels_t(getDrawPercent() *
                          thisType.drawRect_.h);

   //clip, based on randomized direction
   switch(direction_){
   case RIGHT:
      srcLoc = makeRect(0,
                        0,
                        partialW,
                        thisType.drawRect_.h);
      break;
   case DOWN:
      srcLoc = makeRect(0,
                        0,
                        thisType.drawRect_.w,
                        partialH);
      break;
   case LEFT:
      srcLoc = makeRect(thisType.drawRect_.w - partialW,
                        0,
                        partialW,
                        thisType.drawRect_.h);
      drawLoc.x += thisType.drawRect_.w - partialW;
      break;
   case UP:
      srcLoc = makeRect(0,
                        thisType.drawRect_.h - partialH,
                        thisType.drawRect_.w,
                        partialH);
      drawLoc.y += thisType.drawRect_.h - partialH;
      break;
   default:
      assert(false);
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

      for (int i = 0; i != particlesToDraw; ++i){
         //calculate initial co-ords
         pixels_t x = 0, y = 0;
         switch(direction_){
         case UP:
            x = loc_.x + 
                type().drawRect_.x +
                rand() % type().drawRect_.w;
            y = loc_.y + 
                type().drawRect_.y +
                pixels_t((1.0 - drawPercent_) * type().drawRect_.h);
            break;
         case DOWN:
            x = loc_.x + 
                type().drawRect_.x +
                rand() % type().drawRect_.w;
            y = loc_.y + 
                type().drawRect_.y +
                pixels_t(drawPercent_ * type().drawRect_.h);
            break;
         case LEFT:
            x = loc_.x + 
                type().drawRect_.x +
                pixels_t((1.0 - drawPercent_) * type().drawRect_.w);
            y = loc_.y + 
                type().drawRect_.y +
                rand() % type().drawRect_.h;
            break;
         case RIGHT:
            x = loc_.x + 
                type().drawRect_.x +
                pixels_t(drawPercent_ * type().drawRect_.w);
            y = loc_.y + 
                type().drawRect_.y +
                rand() % type().drawRect_.h;
         }

         //add
         game_->particles.push_back(Particle(x, y));
      }
   }
}

double Building::getDrawPercent() const{
   return drawPercent_;
}

int Building::getColor() const{
   return player_;
}

EntityTypeID Building::classID() const{
   return BUILDING;
}

bool Building::selectable() const{
   return
      finished_ &&
      player_ == HUMAN_PLAYER;
}

typeNum_t Building::getPlayer() const{
   return player_;
}