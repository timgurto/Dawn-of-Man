// (C) 2009-2010 Tim Gurto

#ifndef PLAYER_H
#define PLAYER_H

#include "SDL.h"

//Represents one player, be it the human or a
//computer opponent
class Player{
public:
   //the color of this player's entities
   Uint32 color;

   Player(Uint32 playerColor);
};

#endif