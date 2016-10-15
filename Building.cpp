// (C) 2009 Tim Gurto

#include <list>
#include <cassert>
#include "Building.h"
#include "GameData.h"
#include "Debug.h"
#include "Particle.h"

extern Debug debug;

Building::Building(typeNum_t type, const Point &loc, progress_t progress):
Entity(type, loc),
progress_(progress),
finished(false),
drawPercent(EMPTY),
particlesToDraw(0){}

const EntityType &Building::type() const{
   return game_->buildingTypes[type_];
}

void Building::draw(SDL_Surface *screen) const{
   const EntityType &thisType = type();
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;

   SDL_Rect srcLoc;;
   pixels_t
      partialW = pixels_t(getDrawPercent() *
                          thisType.drawRect_.w),
      partialH = pixels_t(getDrawPercent() *
                          thisType.drawRect_.h);

   //clip, based on randomized direction
   switch(direction){
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
   
   if (!MASK_BEFORE_CLIP && ENTITY_MASKS)
      SDL_BlitSurface(thisType.mask, &srcLoc, screen, &drawLoc);

   SDL_BlitSurface(thisType.surface, &srcLoc, screen, &drawLoc);
}

void Building::drawLater() const{
   for (particles_t::const_iterator it = particles.begin();
      it != particles.end(); ++it){
      it->draw();
   }
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

   for (particles_t::iterator it = particles.begin();
      it != particles.end(); ++it){
      it->progress();
      if (it->expired()){
         it = particles.erase(it);
         if (it == particles.end())
            break;
      }
   }

   if (!finished)
      particlesToDraw += 0.05 * 
                         Particle::DECAY *
                         Particle::PARTICLE_COUNT;
                         /*PROGRESS_PER_CALC;*/
   int particlesDrawn = int(particlesToDraw); //round off
   particlesToDraw -= particlesDrawn;

   for (int i = 0; i != particlesDrawn; ++i){
      //calculate co-ords
      pixels_t x = 0, y = 0;
      switch(direction){
      case UP:
         x = loc_.x + 
             type().drawRect_.x +
             rand() % type().drawRect_.w;
         y = loc_.y + 
             type().drawRect_.y +
             pixels_t((1.0 - drawPercent) * type().drawRect_.h);
         break;
      case DOWN:
         x = loc_.x + 
             type().drawRect_.x +
             rand() % type().drawRect_.w;
         y = loc_.y + 
             type().drawRect_.y +
             pixels_t(drawPercent * type().drawRect_.h);
         break;
      case LEFT:
         x = loc_.x + 
             type().drawRect_.x +
             pixels_t((1.0 - drawPercent) * type().drawRect_.w);
         y = loc_.y + 
             type().drawRect_.y +
             rand() % type().drawRect_.h;
         break;
      case RIGHT:
         x = loc_.x + 
             type().drawRect_.x +
             pixels_t(drawPercent * type().drawRect_.w);
         y = loc_.y + 
             type().drawRect_.y +
             rand() % type().drawRect_.h;
      }

      //add
      particles.push_back(Particle(x, y));
   }
}

float Building::getDrawPercent() const{
   return drawPercent;
}