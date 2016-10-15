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
finished_(progress >= game_->unitTypes[type].maxProgress_),
drawPercent_(1.0f *
            progress_ /
            game_->unitTypes[type].maxProgress_){}

const EntityType &Unit::type() const{
   return game_->unitTypes[type_];
}


void Unit::draw(SDL_Surface *screen) const{
   const UnitType &thisType = dynamic_cast<const UnitType &>(type());
   SDL_Rect dstLoc = getDrawRect();
   int srcX = frame_ * thisType.drawRect_.w;
   SDL_Rect srcLoc = makeRect(srcX, 0,
                              thisType.drawRect_.w,
                              thisType.drawRect_.h);
   colorBlit(getColor(), screen, srcLoc, dstLoc);
   //TODO better health display
   SDL_FillRect(screen, &makeRect(dstLoc.x + game_->map.x, dstLoc.y + game_->map.y, thisType.maxHealth_, 5), BLACK_UINT);
   SDL_FillRect(screen, &makeRect(dstLoc.x + game_->map.x, dstLoc.y + game_->map.y, health_, 5), 0x00fe00);
}

void Unit::tick(double delta){
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
            UnitType &targetType = game_->unitTypes[target.type_];
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

int Unit::getColor() const{
   return player_;
}

EntityTypeID Unit::classID() const{
   return UNIT;
}

bool Unit::selectable() const{
   return
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
   double threshold = game_->unitTypes[type_].speed_ * multiplier;

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
      target_ = targetEntity_->loc_;
   else
      combat_ = false;
}