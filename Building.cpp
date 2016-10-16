// (C) 2009-2010 Tim Gurto

#include <list>
#include <cassert>
#include "SDL.h"
#include "Building.h"
#include "GameData.h"
#include "Debug.h"
#include "Particle.h"
#include "BuildingType.h"
#include "Unit.h"
#include "Player.h"

extern Debug debug;

const progress_t Building::PROGRESS_PER_BUILDER_HIT = 100;

Building::Building(typeNum_t type, const Point &loc,
                   typeNum_t player, progress_t progress):
Entity(type, loc),
health_(game_->buildingTypes[type].maxHealth_),
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

   //draw footprint if construction hasn't begun
   if (progress_ == 0){
      SDL_Rect altDrawLoc =
         loc_ +
         thisType.baseRect_ +
         locRect(game_->map);
      SDL_BlitSurface(diagGreen_, &dimRect(getBaseRect()),
      screen, &altDrawLoc);
   }else
      colorBlit(getColor(), screen, srcLoc, drawLoc,
         !finished_); //partial
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

bool Building::targetable() const{
   return player_ != HUMAN_PLAYER || !finished_;
}

typeNum_t Building::getPlayer() const{
   return player_;
}

void Building::progressConstruction(){
   const BuildingType &thisType =
      (const BuildingType &) type();
   progress_ += PROGRESS_PER_BUILDER_HIT;
   if (progress_ >= thisType.maxProgress_){
      finished_ = true;
      drawPercent_ = FULL;
   }else
      drawPercent_ = 1.0 * progress_ /
                     thisType.maxProgress_;

   //add particles
   int particlesToDraw = int(1.0 * rand() / RAND_MAX +
                             0.1 *
                             Particle::PARTICLE_COUNT *
                             Particle::DECAY);
   addParticles(particlesToDraw);


   if (finished_){
      //remove builders' targets
      for (entities_t::iterator it = game_->entities.begin();
           it != game_->entities.end(); ++it)
         if ((*it)->classID() == ENT_UNIT){
            Unit &unit = (Unit &)(**it);
            if (unit.getTargetEntity() == this &&
                unit.isBuilder())
               unit.setTarget(0, (*it)->getLoc());
         }
      //register with player
      game_->players[player_].buildBuilding(typeIndex_);
   }
}

bool Building::isFinished() const{
   return finished_;
}

damage_t Building::getArmor() const{
   return
      game_->buildingTypes[typeIndex_].armor_;
}

damage_t Building::getHealth() const{
   return health_;
}

void Building::removeHealth(damage_t damage){
   health_ -= damage;
}