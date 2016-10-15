// (C) 2010 Tim Gurto

#include <queue>
#include <cmath>
#include "misc.h"
#include "GameData.h"
#include "Unit.h"
#include "UnitType.h"

Unit::Unit(typeNum_t type, const Point &loc,
           typeNum_t player):
Entity(type, loc),
player_(player),
moving_(false){}

const EntityType &Unit::type() const{
   return game_->unitTypes[type_];
}

void Unit::tick(double delta){
   if (moving_){
      //keep them sorted

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

      double speed = delta * game_->unitTypes[type_].speed_;
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

      const SDL_Rect &base = type().baseRect_;

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
      if (atTarget())
         moving_ = false;
      
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

void Unit::setTarget(Point target){
   target_ = target;
   moving_ = true;
}

bool Unit::atTarget(){
   pixels_t threshold = game_->unitTypes[type_].speed_ / 2;

   if (loc_.x - target_.x > threshold ||
       target_.x - loc_.x > threshold)
      return false;
   if (loc_.y - target_.y > threshold ||
       target_.y - loc_.y > threshold)
      return false;

   return true;
}