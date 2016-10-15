// (C) 2009-2010 Tim Gurto

#ifndef PLAYER_H
#define PLAYER_H

#include "SDL.h"
#include "Resources.h"

//Represents one player, be it the human or a
//computer opponent
class Player{

   //the color of this player's entities
   Uint32 color_;

   //the player's resource stockpiles
   Resources resources_;

public:
   Player(Uint32 color);

   void addResources(const Resources &r);

   bool sufficientResources(const Resources &r);

   //get
   Uint32 getColor() const;
};

#endif