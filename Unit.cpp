// (C) 2010 Tim Gurto

#include <queue>
#include <cmath>
#include <cassert>
#include "misc.h"
#include "GameData.h"
#include "Unit.h"
#include "UnitType.h"
#include "Debug.h"

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
drawPercent_(1.0f *
            progress_ /
            game_->unitTypes[type].maxProgress_){}

const EntityType &Unit::type() const{
   return game_->unitTypes[typeIndex_];
}


void Unit::draw(SDL_Surface *screen) const{
   const UnitType &thisType = (const UnitType &)(type());
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;

   pixels_t
      partialW = pixels_t(getDrawPercent() *
                          thisType.drawRect_.w),
      partialH = pixels_t(getDrawPercent() *
                          thisType.drawRect_.h);

   //clip, based on randomized direction
   SDL_Rect srcLoc = getSrcClip(partialW, partialH, frame_);

   switch(direction_){
   case LEFT:
      drawLoc.x += thisType.drawRect_.w - partialW;
      break;
   case UP:
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
                                delta * Particle::DECAY *
                                1);//delta);
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

         //check whether target has been reached
         if (targetEntity_ != 0){
            combat_ = atTarget();
            moving_ = !combat_;
         }else if (atTarget())
            moving_ = false;
      }

      //progress frame
      if (combat_){
         //debug("in combat");
         combatFrameCounter_ += delta;
         if (combatFrameCounter_ >= thisType.maxCombatFrameCounter_){
            combatFrameCounter_ -= (thisType.maxCombatFrameCounter_ +
                                    thisType.combatWait_);
            debug("hit");
            switch (targetEntity_->classID()){
            case UNIT:
               Unit &target = (Unit &)(*targetEntity_);
               UnitType &targetType = game_->unitTypes[target.typeIndex_];
               damage_t damage = thisType.attack_ - targetType.armor_;
               if (damage > target.health_)
                  target.kill();
               else
                  target.health_ -= damage;
               break;
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
   return UNIT;
}

bool Unit::selectable() const{
   return
      finished_ &&
      player_ == HUMAN_PLAYER;
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
   moving_ = !atTarget();
}

bool Unit::atTarget(){
   double multiplier = (targetEntity_ == 0) ? 0.5 : 8;
   double threshold = game_->unitTypes[typeIndex_].speed_ * multiplier;

   if (loc_.x - target_.x > threshold ||
       target_.x - loc_.x > threshold)
      return false;
   if (loc_.y - target_.y > threshold ||
       target_.y - loc_.y > threshold)
      return false;

   return true;
}

void Unit::updateTarget(){
   if (targetEntity_ != 0)
      target_ = targetEntity_->getLoc();
   else
      combat_ = false;
}