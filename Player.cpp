// (C) 2009-2010 Tim Gurto

#include "Player.h"
#include "Debug.h"

extern Debug debug;

Player::Player(Uint32 color):
color_(color){}

void Player::addResources(const Resources &r){
   resources_ += r;
   debug(resources_);
}

Uint32 Player::getColor() const{
   return color_;
}

bool Player::sufficientResources(const Resources &r){
   return resources_ >= r;
}