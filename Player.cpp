// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <vector>
#include "Player.h"
#include "Debug.h"
#include "GameData.h"

extern Debug debug;

GameData *Player::game_ = 0;

Player::Player(Uint32 color,
               const techsResearched_t &techsResearched):
color_(color),
bonuses_(),
techsResearched_(techsResearched){
   resourcesString_ = resources_.str();
   for (size_t i = 0; i != techsResearched.size(); ++i)
      if (techsResearched_[i])
         bonuses_ += game_->techs[i].getBonuses();
}

void Player::init(GameData *game){
   game_ = game;
}

void Player::addResources(const Resources &r){
   resources_ += r;
   resourcesString_ = resources_.str();
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

bool Player::isTechResearched(typeNum_t i) const{
   return techsResearched_[i];
}

void Player::researchTech(typeNum_t index){
   assert(!techsResearched_[index]);
   techsResearched_[index] = true;
}