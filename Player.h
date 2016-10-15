// (C) 2009-2010 Tim Gurto

#ifndef PLAYER_H
#define PLAYER_H

#include "SDL.h"
#include "Resources.h"
#include "TechBonuses.h"

//Represents one player, be it the human or a
//computer opponent
class Player{

   //the color of this player's entities
   Uint32 color_;

   //the player's resource stockpiles
   Resources resources_;

   //So that it doesn't have to be recalculated constantly
   std::string resourcesString_;

   //bonuses from researched techs
   TechBonuses bonuses_;

public:
   //TODO comments
   Player(Uint32 color);

   void addResources(const Resources &r);
   void subtractResources(const Resources &r);

   bool sufficientResources(const Resources &r) const;

   //get
   Uint32 getColor() const;
   std::string getResources() const;
   const TechBonuses &getBonuses() const;
};


#endif