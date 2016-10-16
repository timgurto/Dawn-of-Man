// (C) 2009, 2010 Tim Gurto

#include <cassert>
#include <vector>
#include "Player.h"
#include "Debug.h"
#include "GameData.h"
#include "CoreData.h"
#include "UIBar.h"

extern Debug debug;

GameData *Player::game_ = 0;
const CoreData *Player::core_ = 0;
UIBar *Player::buildingsBar_ = 0;

Player::Player(Uint32 color,
               const resources_t &resources,
               const checklist_t &techsResearched):
color_(color),
bonuses_(),
resources_(resources),
techsResearched_(techsResearched),
alive(true),
buildingsBuilt_(core_->buildingTypes.size(), false),
resourcesString_(resources_.str()){
   for (size_t i = 0; i != techsResearched.size(); ++i)
      if (techsResearched_[i])
         bonuses_ += core_->techs[i].getBonuses();
}

void Player::init(const CoreData *core, GameData *game,
                  UIBar *buildingsBar){
   core_ = core;
   game_ = game;
   buildingsBar_ = buildingsBar;   
}

//initialize player ID
void Player::initID(typeNum_t playerID){
   id_ = playerID;
   ai_.initPlayer(playerID);
}

void Player::addResources(const Resources &r){
   resources_ += r;
   resourcesString_ = resources_.str();

   //Give to AI bookkeeper
   ai_.allocateIncome(r);
   //can we afford anything new?
   ai_.checkExpansion();
   ai_.checkMilitary();
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

void Player::researchTech(typeNum_t index){
   assert(!techsResearched_[index]);
   techsResearched_[index] = true;
   bonuses_ += core_->techs[index].getBonuses();
}

void Player::buildBuilding(typeNum_t index, bool recalc){
   buildingsBuilt_[index] = true;
   if (recalc)
      buildingsBar_->calculateRect();
}

void Player::godMode(){
   if (bonuses_.unitArmor >= 1337) //toggle
      bonuses_.unitArmor -= 1337;
   else
      bonuses_.unitArmor += 1337;
}