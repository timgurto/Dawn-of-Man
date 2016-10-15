// (C) 2009 Tim Gurto

#ifndef ENTITY_H
#define ENTITY_H

#include "SDL.h"
#include "Point.h"
#include "globals.h"

struct GameData;
class EntityType;

//An "instance" of an EntityType.  Basically, each entity
//is an on-screen, in-game element.
class Entity{
   friend class EntityPtr;
protected:
   //type; index in vector of related EntityTypes
   typeNum_t type_;
   Point loc_; //location

   //direction of clipping for partial drawing
   short direction;
   static const GameData *game_; //static pointer to game

public:
   Entity(typeNum_t type, const Point &loc);
   bool operator<(const Entity &rhs) const;
   SDL_Rect getBaseRect();

   //Returns the EntityType object that this entity
   //is an "instance" of
   const virtual EntityType &type() const = 0;

   //Draws the entity, as well as a green mask behind
   //it if ENTITY_MASKS is true
   void draw(SDL_Surface *screen) const;
   
   //Any changes that need to be made to the entity
   //by the game loop
   virtual void tick();

   //Initializes the class' static pointer
   static void setGame(GameData *game);

   //How much of the entity should be drawn.
   //FULL (1) by default
   virtual float getDrawPercent() const;
};

#endif