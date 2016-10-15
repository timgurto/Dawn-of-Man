// (C) 2009-2010 Tim Gurto

#include "Player.h"
#include "Debug.h"

extern Debug debug;

Player::Player(Uint32 color):
color_(color){
   resourcesString_ = resources_.str();
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