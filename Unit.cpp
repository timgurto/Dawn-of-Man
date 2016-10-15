// (C) 2010 Tim Gurto

#include <queue>
#include <cmath>
#include <cassert>
#include "misc.h"
#include "GameData.h"
#include "Unit.h"
#include "UnitType.h"
#include "Debug.h"
#include "Building.h"
#include "ResourceNode.h"
#include "ResourceNodeType.h"

extern Debug debug;

Unit::Unit(typeNum_t type, const Point &loc,
           typeNum_t player, progress_t progress):
Entity(type, loc),
player_(player),
moving_(false),
combat_(false),
frameCounter_(rand() % game_->unitTypes[type].maxFrameCounter_),
combatFrameCounter_(0),
frame_(0),
targetEntity_(0),
health_(game_->unitTypes[type].maxHealth_),
progress_(progress),
finished_(progress >= game_->unitTypes[type].maxProgress_),
drawPercent_(game_->unitTypes[type].maxProgress_ == 0 ?
                1.0 :
                1.0 * progress_ /
                game_->unitTypes[type].maxProgress_){}

const EntityType &Unit::type() const{
   return game_->unitTypes[typeIndex_];
}


void Unit::draw(SDL_Surface *screen) const{
   const UnitType &thisType = (const UnitType &)(type());
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;

   pixels_t
      partialW = pixels_t(drawPercent_ *
                          thisType.drawRect_.w),
      partialH = pixels_t(drawPercent_ *
                          thisType.drawRect_.h);

   //clip, based on randomized direction
   SDL_Rect srcLoc = getSrcClip(partialW, partialH, frame_);

   switch(direction_){
   case DIR_LEFT:
      drawLoc.x += thisType.drawRect_.w - partialW;
      break;
   case DIR_UP:
      drawLoc.y += thisType.drawRect_.h - partialH;
   }

   colorBlit(player_, screen, srcLoc, drawLoc);



   //TODO better health display
   if (finished_){
      SDL_FillRect(screen,
                   &makeRect(drawLoc.x + game_->map.x - 1,
                             drawLoc.y + game_->map.y - 1,
                             thisType.maxHealth_, 5),
                   0x444444);
      SDL_FillRect(screen,
                   &makeRect(drawLoc.x + game_->map.x + 1,
                             drawLoc.y + game_->map.y + 1,
                             thisType.maxHealth_, 5),
                   0xdddddd);
      SDL_FillRect(screen,
                   &makeRect(drawLoc.x + game_->map.x,
                             drawLoc.y + game_->map.y,
                             thisType.maxHealth_, 5),
                   BLACK_UINT);
      SDL_FillRect(screen,
                   &makeRect(drawLoc.x + game_->map.x,
                             drawLoc.y + game_->map.y,
                             health_, 5),
                   getEntityColor(*game_, player_));
   }
}

void Unit::tick(double delta){

   //training
   if (!finished_){
      progress_ += delta * PROGRESS_PER_CALC;
      //debug("progress = ", progress_);
      if (progress_ >= game_->unitTypes[typeIndex_].maxProgress_){
         finished_ = true;
         drawPercent_ = FULL;
      }else
         drawPercent_ = 1.0 * progress_ /
                 game_->unitTypes[typeIndex_].maxProgress_;

      int particlesToDraw = int(1.0 * rand() / RAND_MAX +
                                0.02 * 
                                Particle::PARTICLE_COUNT *
                                delta * Particle::DECAY);
      addParticles(particlesToDraw);

   }else{

      updateTarget();
      const UnitType &thisType = (const UnitType &)(type());

      if (moving_){

         double angle;
         if (target_.x == loc_.x)
            angle =
               (target_.y < loc_.y ?
               1.5 * PI :
               0.5 * PI);
         else{
            double gradient = 1.0 *
               (target_.y - loc_.y) /
               (target_.x - loc_.x);
            angle = atan(gradient);
            if (target_.x < loc_.x){
               if (target_.y > loc_.y)
                  angle += PI;
               else
                  angle -= PI;
            }
         }
         double xMod = cos(angle);
         double yMod = sin(angle);

         double speed = delta * thisType.speed_;
         pixels_t
            xDelta = pixels_t(xMod * speed),
            yDelta = pixels_t(yMod * speed);

         loc_.x += xDelta;
         loc_.y += yDelta;

         // if collision, undo
         if (!isLocationOK()){
            loc_.x -= xDelta;
            loc_.y -= yDelta;
            return;
         }

         if (yMod > 0)
            verticalMovement_ = VM_DOWN;
         else if (yMod < 0)
            verticalMovement_ = VM_UP;
         else
            verticalMovement_ = VM_NONE;

         const SDL_Rect &base = thisType.baseRect_;

         //check in bounds
         if (xMod < 0 &&
             loc_.x + base.x < 0)
            loc_.x = -1 * base.x;
         else if (xMod > 0 &&
                  loc_.x + base.x + base.w > game_->map.w)
            loc_.x = game_->map.w - base.x - base.w;
         if (yMod < 0 &&
             loc_.y + base.y < 0)
            loc_.y = -1 * base.y;
         else if (yMod > 0 &&
                  loc_.y + base.y + base.h > game_->map.h)
            loc_.y = game_->map.h - base.y - base.h;

      }

      //check whether target has been reached
      if (targetEntity_ != 0){
         combat_ = isAtTarget();
         moving_ = !combat_;
      }else if (isAtTarget())
         moving_ = false;

      //progress frame
      if (combat_){
         //debug("in combat");
         combatFrameCounter_ += delta;

         //Combat/construction
         if (combatFrameCounter_ >= thisType.maxCombatFrameCounter_){
            combatFrameCounter_ -= (thisType.maxCombatFrameCounter_ +
                                    thisType.combatWait_);
            //debug("hit");
            switch (targetEntity_->classID()){
            case ENT_BUILDING:
               {//local scope for target
                  Building &target = (Building &)(*targetEntity_);
                  //friendly building: construction
                  if (thisType.builder_ &&
                      target.getPlayer() == player_){
                     target.progressConstruction();
                  }
               }
               break;
            //TODO remove health in target's method
            case ENT_UNIT:
               {//local scope for target, targetType, damage
                  Unit &target = (Unit &)(*targetEntity_);
                  const UnitType &targetType =
                     game_->unitTypes[target.typeIndex_];
                  damage_t damage = thisType.attack_ - targetType.armor_;
                  if (damage > target.health_)
                     target.kill();
                  else
                     target.health_ -= damage;
                  break;
               }
            case ENT_RESOURCE_NODE:
               {//local scope for target, targetType
                  ResourceNode &target = (ResourceNode &)(*targetEntity_);
                  Resources yield = target.harvest();
                  game_->players[player_].addResources(yield);
               }
            }
         }
         if (combatFrameCounter_ < 0)
            frame_ = thisType.frameCount_;
         else
            frame_ = int(combatFrameCounter_ /
                         thisType.maxCombatFrameCounter_ *
                         thisType.combatFrameCount_ +
                         thisType.frameCount_);
      }else{
            frameCounter_ = modulo(frameCounter_ + delta,
                                   thisType.maxFrameCounter_);
         frame_ = moving_ ?
                     int(frameCounter_ /
                         thisType.maxFrameCounter_ *
                         thisType.frameCount_) :
                     0;
      }
   }
}

double Unit::getDrawPercent() const{
   return drawPercent_;
}

int Unit::getColor() const{
   return player_;
}

EntityTypeID Unit::classID() const{
   return ENT_UNIT;
}

bool Unit::selectable() const{
   return
      finished_ &&
      player_ == HUMAN_PLAYER;
}

bool Unit::targetable() const{
   return true;
}

typeNum_t Unit::getPlayer() const{
   return player_;
}

void Unit::setTarget(Entity *targetEntity, Point loc){
   targetEntity_ = targetEntity;
   if (targetEntity == 0)
      target_ = loc;
   else
      updateTarget();
   moving_ = !isAtTarget();
}

bool Unit::isAtTarget() const{
   pixels_t margin = game_->unitTypes[typeIndex_].speed_;
   if (targetEntity_ == 0)
      //straight distance to a point
      return (distance(loc_, target_) < margin);
   else{
      SDL_Rect baseRect = getBaseRect();
      SDL_Rect targetRect = targetEntity_->getBaseRect();
      //check whether diagonal distance is close enough
      if (collision(targetRect, baseRect +
                                Point(margin, margin)))
         return true;
      if (collision(targetRect, baseRect +
                                Point(margin, -1 * margin)))
         return true;
      if (collision(targetRect, baseRect +
                                Point(-1 * margin, margin)))
         return true;
      if (collision(targetRect, baseRect -
                                Point(margin, margin)))
         return true;

      return false;
   }

   assert(false);
   return false;
}

void Unit::updateTarget(){
   if (targetEntity_ != 0)
      target_ = targetEntity_->getLoc();
   else
      combat_ = false;
}

bool Unit::isBuilder() const{
   return game_->unitTypes[typeIndex_].builder_;
}

bool Unit::isGatherer() const{
   return game_->unitTypes[typeIndex_].gatherer_;
}

Entity *Unit::getTargetEntity() const{
   return targetEntity_;
}