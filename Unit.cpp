// (C) 2010 Tim Gurto

#include <queue>
#include <cmath>
#include <map>
#include <cassert>
#include "SDL_mixer.h"
#include "misc.h"
#include "GameData.h"
#include "Unit.h"
#include "UnitType.h"
#include "Debug.h"
#include "Building.h"
#include "ResourceNode.h"
#include "ResourceNodeType.h"
#include "TechBonuses.h"
#include "Player.h"

extern Debug debug;

const pixels_t Unit::PATH_GRID_SIZE = 50;

Unit::Unit(typeNum_t type, const Point &loc,
           typeNum_t player, progress_t progress):
Entity(type, loc),
player_(player),
moving_(false),
combat_(false),
pathJustSet_(false),
frameCounter_(rand() % core_->unitTypes[type].maxFrameCounter_),
combatFrameCounter_(0),
frame_(0),
targetEntity_(0),
health_(core_->unitTypes[type].maxHealth_),
progress_(progress),
finished_(progress >= core_->unitTypes[type].maxProgress_),
drawPercent_(core_->unitTypes[type].maxProgress_ == 0 ?
                1.0 :
                1.0 * progress_ /
                core_->unitTypes[type].maxProgress_){}

const EntityType &Unit::type() const{
   return core_->unitTypes[typeIndex_];
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

   colorBlit(player_, screen, srcLoc, drawLoc,
             //partial, if not finished
             !finished_);
   //debug("health / max = ", 1.0f * health_ / thisType.maxHealth_);
}

void Unit::tick(double delta){
   const UnitType &thisType = core_->unitTypes[typeIndex_];

   //training
   if (!finished_){
      progress_ += delta * (PROGRESS_PER_CALC + game_->
                            players[player_].getBonuses().
                            trainingSpeed);
      //debug("progress = ", progress_);
      if (progress_ >= thisType.maxProgress_){
         finished_ = true;
         drawPercent_ = FULL;
         playSound(thisType.sound_);
      }else
         drawPercent_ = 1.0 * progress_ /
                        thisType.maxProgress_;

      int particlesToDraw = int(1.0 * rand() / RAND_MAX +
                                0.02 * 
                                Particle::PARTICLE_COUNT *
                                delta * Particle::DECAY);
      addParticles(particlesToDraw);

   }else{
      if (pathJustSet_)
         pathJustSet_ = false;
      else{

         if (moving_){
            const Point &targetPoint = path_.front();

            double angle;
            if (targetPoint.x == loc_.x)
               angle =
                  (targetPoint.y < loc_.y ?
                  1.5 * PI :
                  0.5 * PI);
            else{
               double gradient = 1.0 *
                  (targetPoint.y - loc_.y) /
                  (targetPoint.x - loc_.x);
               angle = atan(gradient);
               if (targetPoint.x < loc_.x){
                  if (targetPoint.y > loc_.y)
                     angle += PI;
                  else
                     angle -= PI;
               }
            }
            double xMod = cos(angle);
            double yMod = sin(angle);

            double speed = delta * getSpeed();

            pixels_t
               xDelta = pixels_t(xMod * speed),
               yDelta = pixels_t(yMod * speed);

            loc_.x += xDelta;
            loc_.y += yDelta;

            // if collision, undo
            if (!isLocationOK()){
               loc_.x -= xDelta;
               loc_.y -= yDelta;
               findPath();
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
         if (path_.size() > 1){
            if (isAtTarget())
               path_.pop();
         }else{
            if (targetEntity_){
               //if the target entity has moved
               if (!isAtTarget() && target_ != targetEntity_->getLoc()){
                  //updateTarget();
                  path_.pop();
                  assert (path_.empty());
                  path_.push(targetEntity_->getLoc());

                  combat_ = false;
                  moving_ = true;
               }else{
                  combat_ = isAtTarget();
                  moving_ = !combat_;
               }
            }else if (isAtTarget())
               moving_ = false;
         }

         //progress frame
         if (combat_){
            //debug("in combat");
            combatFrameCounter_ += delta;

            //Combat/construction
            if (combatFrameCounter_ >= thisType.maxCombatFrameCounter_){
               combatFrameCounter_ -= (thisType.maxCombatFrameCounter_ +
                                       thisType.combatWait_);
               playSound(thisType.getHitSound());
               //debug("hit");
               switch (targetEntity_->classID()){
               case ENT_BUILDING:
                  {//local scope for target
                     Building &target = (Building &)(*targetEntity_);
                     //friendly building: construction
                     typeNum_t targetPlayer = target.getPlayer();
                     if (thisType.builder_ && targetPlayer == player_){
                        target.progressConstruction();
                     //enemy building: attack
                     }else if(targetPlayer != player_){
                        damage_t damage = getAttack() - target.getArmor();
                        if (damage > target.getHealth())
                           target.kill();
                        else
                           target.removeHealth(damage);
                     }
                  }
                  break;
               //TODO remove health in target's method
               case ENT_UNIT:
                  {//local scope for target, targetType, damage
                     Unit &target = (Unit &)(*targetEntity_);
                     damage_t damage = getAttack() - target.getArmor();
                     if (damage > target.getHealth())
                        target.kill();
                     else{
                        target.health_ -= damage;
                        if (!target.getTargetEntity() &&
                            !target.moving_)
                           target.setTarget(this);
                     }
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
}
//TODO ?
void Unit::setTarget(Entity *targetEntity, Point loc){
   targetEntity_ = targetEntity;
   if (!targetEntity){
      //keep target in-bounds
      if (loc.x < 0)
         loc.x = 0;
      else if (loc.x >= game_->map.w)
         loc.x = game_->map.w - 1;
      if (loc.y < 0)
         loc.y = 0;
      else if (loc.y >= game_->map.h)
         loc.y = game_->map.h - 1;

      target_ = loc;
      combat_ = false;
   }else
      updateTarget();
   moving_ = !isAtTarget();
   if (moving_ && !targetEntity){
      findPath();
   }
   //debug("Path clear: ", isPathClear(loc_, target_,
   //                                  *game_, *this));

   //updateTarget();
}

bool Unit::isAtTarget() const{
   pixels_t margin = getSpeed();
   Point target = path_.size() > 1 ?
                  path_.front() :
                  target_;
   //straight distance to a point
   if (path_.size() > 1 || !targetEntity_)
      return (distance(loc_, target) < margin);
   
   //target is an entity
   else{
      SDL_Rect baseRect = getBaseRect();
      SDL_Rect targetRect = targetEntity_->getBaseRect();
      //check whether diagonal distance is close enough
      if (collision(targetRect, baseRect +
                                Point(margin, margin)))
         return true;
      pixels_t negMargin = -1 * margin;
      if (collision(targetRect, baseRect +
                                Point(margin, negMargin)))
         return true;
      if (collision(targetRect, baseRect +
                                Point(negMargin, margin)))
         return true;
      if (collision(targetRect, baseRect -
                                Point(margin, margin)))
         return true;
      return false;
   }

   assert(false);
   return false;
}

bool Unit::isPathClear(const Point &start,
                 const Point &end,
                 double angle) const{
   const EntityType &thisType = type();


   //if not already done, calculate angle
   if (angle == DUMMY_ANGLE)
      if (end.x == start.x)
         angle =
            end.y < start.y ?
            1.5 * PI :
            0.5 * PI;
      else{
         double gradient =
            1.0 *
            (end.y - start.y) /
            (end.x - start.x);
         angle = atan(gradient);
         if (end.x < start.x)
            if (end.y > start.y)
               angle += PI;
            else
               angle -= PI;
      }

   double
      xDelta = cos(angle) * PATH_CHECK_DISTANCE,
      yDelta = sin(angle) * PATH_CHECK_DISTANCE;

   double x = start.x, y = start.y;
   bool finished = false;
   while (!finished){
      //debug("Checking ", x, ",", y);
      if (!noCollision(*game_,
                       thisType,
                       Point(pixels_t(x), pixels_t(y)),
                       this, this->targetEntity_))
         return false;

      x += xDelta;
      y += yDelta;
      finished =
         (start.x < end.x ? x >= end.x : x <= end.x) &&
         (start.y < end.y ? y >= end.y : y <= end.y);
   }
   return true;
}

void Unit::findPath(){
   debug("finding path");
   emptyQueue(path_);
   //if clear path, go straight there
   if (isPathClear(loc_, target_)){
      path_.push(target_);
      pathJustSet_  = true;
      return;
   }

   if (!noCollision(*game_, type(), target_, this, targetEntity_)){
      moving_ = false;
      path_.push(loc_);
      return;
   }

   //path needs to be found
   std::map<Point, bool> explored;
   //set up queue of queues to store branching paths
   typedef std::queue<Point> path_t;
   path_t start;
   start.push(loc_);
   explored[loc_] = true;
   std::queue<path_t> paths;
   paths.push(start);

   do{
      for (int dir = DIR_UP; dir != DIR_MAX; ++dir){
         path_t temp(paths.front());
         Point next = temp.back(); // = current
         switch (dir){
         case DIR_UP:
            next.y -= PATH_GRID_SIZE;
            if (next.y < 0) continue;
            break;
         case DIR_DOWN:
            next.y += PATH_GRID_SIZE;
            if (next.y > game_->map.h) continue;
            break;
         case DIR_LEFT:
            next.x -= PATH_GRID_SIZE;
            if (next.x < 0) continue;
            break;
         case DIR_RIGHT:
            next.x += PATH_GRID_SIZE;
            if (next.x > game_->map.w) continue;
            break;
         }

         //don't repeat nodes
         if (explored[next])
            continue;
         explored[next] = true;


         if (noCollision(*game_, type(), next, this, targetEntity_)){

            //is this the node we're looking for?
            temp.push(next);
            if (distance(next, target_) <= PATH_GRID_SIZE){
               temp.push(target_);
               temp.pop(); //first node is the current location
               path_ = temp;
               moving_ = true;
               pathJustSet_  = true;
               return;
            }
            paths.push(temp);
         }

      }//for each direction
      //discard the path that has now been extended
      if (!paths.empty())
         paths.pop();

   }while (!paths.empty());

   assert (path_.empty());
   if (path_.empty()){
      path_.push(loc_);
      moving_ = false;
   }
   pathJustSet_  = true;
}

void Unit::updateTarget(){
   if (targetEntity_){
      target_ = targetEntity_->getLoc();
      findPath();
   }else
      combat_ = false;

   moving_ = !path_.empty();
}

bool Unit::isBuilder() const{
   return core_->unitTypes[typeIndex_].builder_;
}

bool Unit::isGatherer() const{
   return core_->unitTypes[typeIndex_].gatherer_;
}

Entity *Unit::getTargetEntity() const{
   return targetEntity_;
}

damage_t Unit::getArmor() const{
   return
      core_->unitTypes[typeIndex_].armor_ +
      game_->players[player_].getBonuses().unitArmor;
}

damage_t Unit::getHealth() const{
   return
      health_ +
      game_->players[player_].getBonuses().unitHealth;
}

damage_t Unit::getAttack() const{
   return
      core_->unitTypes[typeIndex_].attack_ +
      game_->players[player_].getBonuses().attack;
}

pixels_t Unit::getSpeed() const{
   return
      core_->unitTypes[typeIndex_].speed_ +
      game_->players[player_].getBonuses().speed;
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
   return player_ != HUMAN_PLAYER;
}

bool Unit::drawBlack() const{
   return
      1.0 * health_ /
      core_->unitTypes[typeIndex_].maxHealth_ <
      ENTITY_BLACK_HEALTH;
}

typeNum_t Unit::getPlayer() const{
   return player_;
}