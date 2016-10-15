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
      
      double gradient =
         (target_.y - loc_.y) /
         (target_.x - loc_.x);
      double angle = atan(gradient);
      if (target_.x < loc_.x){
         if (target_.y > loc_.y)
            angle += PI;
         else
            angle -= PI;
      }
      double xMod = cos(angle);
      double yMod = sin(angle);


      
      //const UnitType &thisType = dynamic_cast<UnitType &>(type()));
      //pixels_t speed = thisType.speed_;
      pixels_t speed = game_->unitTypes[type_].speed_;
      loc_.x += pixels_t(xMod * speed);
      loc_.y += pixels_t(yMod * speed);

      //checkAtTarget();
      
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