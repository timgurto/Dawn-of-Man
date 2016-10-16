// (C) 2009, 2010 Tim Gurto

#include <cassert>
#include <vector>
#include "Player.h"
#include "Debug.h"
#include "GameData.h"
#include "CoreData.h"
#include "UIBar.h"
#include "TechBonuses.h"

extern Debug debug;

GameData *Player::game_ = 0;
const CoreData *Player::core_ = 0;
UIBar *Player::buildingsBar_ = 0;

Player::Player(Uint32 color,
               const Resources &resources,
               const checklist_t &techsResearched,
               const checklist_t &buildingsBuilt,
               typeNum_t playerID):
id_(playerID),
color_(color),
bonuses_(),
resources_(resources),
techsResearched_(techsResearched),
buildingsBuilt_(buildingsBuilt),
alive(true),
resourcesString_(resources_.str()),
initialAIUpdate_(false){
   ai_.initPlayer(playerID);
      ai_.allocateIncome(resources_);
   for (size_t i = 0; i != techsResearched.size(); ++i)
      if (techsResearched_[i])
         bonuses_ += core_->techs[i].getBonuses();
   if (game_)
      ai_.update();
}

void Player::init(const CoreData *core, GameData *game,
                  UIBar *buildingsBar){
   core_ = core;
   game_ = game;
   buildingsBar_ = buildingsBar;
}

void Player::addResources(const Resources &r){
   resources_ += r;
   resourcesString_ = resources_.str();

   //Give to AI bookkeeper
   ai_.allocateIncome(r);
   ai_.update();
}

void Player::subtractResources(const Resources &r){
   resources_ -= r;
   resourcesString_ = resources_.str();
}

Uint32 Player::getColor() const{
   return color_;
}

bool Player::sufficientResources(const Resources &r) const{
   return resources_ >= r;
}

std::string Player::getResources() const{
   return resourcesString_;
}

const TechBonuses &Player::getBonuses() const{
   return bonuses_;
}

bool Player::hasTech(typeNum_t i) const{
   if (i == NO_TYPE)
      return true;
   return techsResearched_[i];
}

bool Player::hasBuilding(typeNum_t i) const{
   if (i == NO_TYPE)
      return true;
   return buildingsBuilt_[i];
}

void Player::registerTech(typeNum_t index){
   assert(!techsResearched_[index]);
   techsResearched_[index] = true;
   bonuses_ += core_->techs[index].getBonuses();
   ai_.update();
}

void Player::registerBuilding(typeNum_t index, bool recalc){
   buildingsBuilt_[index] = true;
   if (recalc)
      buildingsBar_->calculateRect();
   ai_.update();
}

void Player::registerUnit(Unit *unit){
   ai_.dispatchUnit(unit);
}

void Player::godMode(){
   if (bonuses_.unitArmor >= 1337) //toggle
      bonuses_.unitArmor -= 1337;
   else
      bonuses_.unitArmor += 1337;
}

void Player::tick(){
   if (!initialAIUpdate_){
      ai_.dispatchAllUnits();
      ai_.update();
      initialAIUpdate_ = true;
   }
}

void Player::idleGatherer(Unit *unit, const ResourceNode *ignore){
   ai_.dispatchUnit(unit, ignore);
}