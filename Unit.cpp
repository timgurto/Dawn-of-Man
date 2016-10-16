// (C) 2010 Tim Gurto

#include <queue>
#include <cmath>
#include <map>
#include <cassert>
#include <sstream>
#include "misc.h"
#include "util.h"
#include "GameData.h"
#include "Unit.h"
#include "UnitType.h"
#include "Debug.h"
#include "Building.h"
#include "ResourceNode.h"
#include "ResourceNodeType.h"
#include "TechBonuses.h"
#include "Player.h"
#include "Sound.h"

extern Debug debug;

const pixels_t Unit::PATH_GRID_SIZE = 30;
const pixels_t Unit:: AUTO_ATTACK_DISTANCE = 200;

Unit::Unit(typeNum_t type, const Point &loc,
           typeNum_t player, progress_t progress):
Entity(type, loc),
player_(player),
moving_(false),
combat_(false),
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
                core_->unitTypes[type].maxProgress_),
controlGroup(CONTROL_NONE){}

const EntityType &Unit::type() const{
   return core_->unitTypes[typeIndex_];
}


void Unit::draw(Surface &screen) const{
   const UnitType &thisType = (const UnitType &)(type());
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;
   SDL_Rect srcLoc;

   if (drawPercent_ < FULL){
      pixels_t
         partialW = pixels_t(drawPercent_ *
                             thisType.drawRect_.w),
         partialH = pixels_t(drawPercent_ *
                             thisType.drawRect_.h);

      //clip, based on randomized direction
      srcLoc = getSrcClip(partialW, partialH, frame_);

      switch(direction_){
      case DIR_LEFT:
         drawLoc.x += thisType.drawRect_.w - partialW;
         break;
      case DIR_UP:
         drawLoc.y += thisType.drawRect_.h - partialH;
         break;
      }
   }else
      srcLoc = makeRect((frame_) * thisType.drawRect_.w,
                        0,
                        thisType.drawRect_.w,
                        thisType.drawRect_.h);


   colorBlit(player_, screen, srcLoc, drawLoc,
             //partial, if not finished
             !finished_);
   //debug("health / max = ", 1.0f * health_ / thisType.maxHealth_);

   if (DEBUG)
      screen.fill(getEntityColor(*game_, getColor()),
                  &makeRect(target_.x + game_->map.x - 2,
                            target_.y + game_->map.y - 2,
                            5, 5));
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
         thisType.sound_.play();
         game_->players[player_].registerUnit(this);
      }else
         drawPercent_ = 1.0 * progress_ /
                        thisType.maxProgress_;

      int particlesToDraw = int(1.0 * rand() / RAND_MAX +
                                0.02 * 
                                Particle::PARTICLE_COUNT *
                                delta * Particle::DECAY);
      addParticles(particlesToDraw);

   }else{

      //auto-attack nearby enemies if not busy
      if (!moving_ && !targetEntity_ && thisType.autoAttack_){
         Entity *nearbyEnemy = findNearbyEnemy();
         if (nearbyEnemy)
            setTarget(findNearbyEnemy());
      }

      if (targetEntity_ && !combat_ && path_.empty())
         findPath();

      //movement stuff
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

         double distanceToTarget = distance(loc_, targetPoint);
         if (speed > distanceToTarget)
            speed = distanceToTarget;

         pixels_t
            xDelta = pixels_t(xMod * speed),
            yDelta = pixels_t(yMod * speed);

         loc_.x += xDelta;
         loc_.y += yDelta;

         // if collision, undo
         if (!isLocationOK()){
            loc_.x -= xDelta;
            loc_.y -= yDelta;
            //obstacle too close - finer search
            findPath(pixels_t(PATH_GRID_SIZE * 0.4));
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
               emptyQueue(path_);
               updateTarget();
               combat_ = false;
               moving_ = true;
            }else{
               combat_ = isAtTarget();
               moving_ = !combat_;
            }
         }else if (isAtTarget())
            moving_ = false;
      }
      
      if (combat_ && !isAtTarget()){
         findPath();
      }

      //progress frame
      if (targetEntity_ && combat_){
         //debug("in combat");
         combatFrameCounter_ += delta;

         //Combat/construction
         if (combatFrameCounter_ >= thisType.maxCombatFrameCounter_){
            combatFrameCounter_ -= (thisType.maxCombatFrameCounter_ +
                                    thisType.combatWait_);
            thisType.getHitSound().play();
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
                     if (target.getArmor() < getAttack()){
                        damage_t damage = getAttack() - target.getArmor();
                        if (damage >= target.getHealth())
                           target.kill();
                        else
                           target.removeHealth(damage);
                     }
                  }
               }
               break;
            case ENT_UNIT:
               {//local scope for target, targetType, damage
                  Unit &target = (Unit &)(*targetEntity_);
                  if (target.getArmor() < getAttack()){
                     damage_t damage = getAttack() - target.getArmor();
                     debug(getAttack(), "attack; ", target.getArmor(), "armor");
                     debug("Damage: ", damage);
                     if (damage >= target.getHealth())
                        target.kill();
                     else{
                        target.removeHealth(damage);
                        if (!(target.targetEntity_ ||
                              target.moving_ ||
                              target.combat_))
                           target.setTarget(this);
                     }
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

//sets the target entity, and co-ordinates.  If an
//entity isn't provided, use the specified co-ordinates
void Unit::setTarget(Entity *targetEntity, Point loc){
   targetEntity_ = targetEntity;
   if (!targetEntity){
      //keep target in-bounds
      const SDL_Rect baseRect = type().getBaseRect();
      if (loc.x <= -baseRect.x)
         loc.x = 1 - baseRect.x;
      else if (loc.x >= game_->map.w -
                        baseRect.w -
                        baseRect.x)
         loc.x = game_->map.w -
                 baseRect.w -
                 baseRect.x -
                 1;
      if (loc.y <= -baseRect.y)
         loc.y = 1 - baseRect.y;
      else if (loc.y >= game_->map.h -
                        baseRect.h -
                        baseRect.y)
         loc.y = game_->map.h -
                 baseRect.h -
                 baseRect.y -
                 1;

      target_ = loc;
      combat_ = false;
   }else
      updateTarget();
   moving_ = !isAtTarget();
   if (moving_){
      findPath();
   }
}

bool Unit::isAtTarget() const{
   SDL_Rect baseRect = getBaseRect();
   pixels_t margin = max(baseRect.w, baseRect.h) / 2; //1.5 * getSpeed();
   Point target = path_.size() > 1 ?
                     path_.front() :
                     target_;
   //straight distance to a point
   if (path_.size() > 1 || !targetEntity_)
      return (distance(loc_, target) < margin);
   
   //target is an entity
   else{
      SDL_Rect targetRect = targetEntity_->getBaseRect();
      //check whether diagonal distance is close enough
      if (collision(targetRect, baseRect +
                                Point(margin, margin)))
         return true;
      pixels_t negMargin = -margin;
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
}

//Whether the unit would have a clear path between
//the two specified points
bool Unit::isPathClear(const Point &start,
                 const Point &end) const{
   const EntityType &thisType = type();

   //calculate angle
   double angle;
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
      if (end.x < start.x){
         if (end.y > start.y)
            angle += PI;
         else
            angle -= PI;
      }
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

//calculates a path to the target, and fills the path_
//with the co-ordinates of each sub-target
void Unit::findPath(pixels_t gridSize){
   debug("finding path");

   //if already there
   if (isAtTarget()){
      moving_ = false;
      combat_ = true;
      return;
   }

   emptyQueue(path_);
   
   //if clear path, go straight there
   if (isPathClear(loc_, target_)){
      path_.push(target_);
      moving_ = true;
      combat_ = false;
      return;
   }

   //target is surrounded
   if (!targetEntity_ &&
       !noCollision(*game_, type(), target_, this, targetEntity_)){
      moving_ = false;
      path_.push(target_);
      target_ = loc_;
      targetEntity_ = 0;
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
            next.y -= gridSize;
            if (next.y < 0) continue;
            break;
         case DIR_DOWN:
            next.y += gridSize;
            if (next.y > game_->map.h) continue;
            break;
         case DIR_LEFT:
            next.x -= gridSize;
            if (next.x < 0) continue;
            break;
         case DIR_RIGHT:
            next.x += gridSize;
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
            //if (distance(next, target_) <= gridSize){
            if (isPathClear(next, target_)){
               temp.pop(); //first node is the current location
               temp.push(target_);
               path_ = temp;
               moving_ = true;
               combat_ = false;
               return;
            }
            paths.push(temp);
         }

      }//for each direction
      //discard the path that has now been extended
      if (!paths.empty())
         paths.pop();

   }while (!paths.empty());

   //no path found
   emptyQueue(path_);
   path_.push(loc_);
   target_ = loc_;
   targetEntity_ = 0;
   moving_ = false;
   combat_ = false;
}

//sets target co-ordinates if targetting an entity
void Unit::updateTarget(){
   if (targetEntity_){
      target_ = targetEntity_->getLoc();
      //findPath();
   }else
      combat_ = false;
   //moving_ = !path_.empty();
   moving_ = !isAtTarget();
}

void Unit::removeHealth(damage_t damage){
   health_ -= damage;
}

Entity *Unit::findNearbyEnemy(){
   //TODO only search nearby entities
   ITERATE(entities_t::const_iterator, game_->entities, it){
      typeNum_t player = (*it)->getPlayer();
      if (player != NO_TYPE && //entity has a player associated with it
          player != player_ && //enemy
          player != NATURE_PLAYER) //deer never hurt anyone
         if (distance(loc_, (*it)->getLoc()) <= AUTO_ATTACK_DISTANCE)
            return *it;
   }
   return 0;
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
   damage_t bonus = game_->players[player_]
               .getBonuses().unitHealth;
   return
      1.0 *  (health_ + bonus) /
      (core_->unitTypes[typeIndex_].maxHealth_ + bonus)<
      ENTITY_BLACK_HEALTH;
}

typeNum_t Unit::getPlayer() const{
   return player_;
}

std::string Unit::getHelp() const{
   const UnitType &thisType = (const UnitType &)type();
   std::ostringstream os;
   if (player_ != HUMAN_PLAYER)
      os << "Enemy ";
   os << thisType.name_;
   os << " - " << getHealth() << " health";
   if (health_ < thisType.maxHealth_)
      os << " remaining";
   if (DEBUG)
      os << " | group: " << controlGroup - CONTROL_NONE
         << " | armor: " << getArmor();
   return os.str();
}