#include "Player.h"

Player::Player(Uint32 color):
color_(color){}

Uint32 Player::getColor() const{
   return color_;
}