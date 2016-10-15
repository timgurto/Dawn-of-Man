// (C) 2009-2010 Tim Gurto

#ifndef ENTITY_H
#define ENTITY_H

#include <list>
#include "SDL.h"
#include "globals.h"
#include "Point.h"

struct GameData;
class EntityType;

//An "instance" of an EntityType.  Basically, each entity
//is an on-screen, in-game element.
class Entity{
   friend class EntityPtr;
protected: //everything should be a derived class

   //type; index in vector of related EntityTypes
   typeNum_t type_;

   //direction of clipping for partial drawing
   Direction direction;

   Point loc_; //location

   //used in drawing; static to avoid having to
   //recreate it constantly
   static SDL_Surface *colorTemp;

   static GameData *game_; //static pointer to game
   static SDL_Surface *screen_;

   //Actually draws the sprite, after draw() figures out
   //the two rectangle parameters
   void shadowBlit(SDL_Rect *srcLoc,
                   SDL_Rect *dstLoc,
                   SDL_Surface *screen) const;

   //blit to the color mask, then blit the color mask
   //to the screen, to create a colored sprite
   void colorBlit(int color, SDL_Surface *surface,
                  SDL_Surface *screen,
                  SDL_Rect &srcLoc,
                  SDL_Rect &dstLoc) const;

public:
   Entity(typeNum_t type, const Point &loc);

   //less than; compares y co-ordinates
   bool operator<(const Entity &rhs) const;
   
   SDL_Rect getBaseRect();

   virtual ~Entity(){}

   //Returns the EntityType object that this entity
   //is an "instance" of
   const virtual EntityType &type() const = 0;

   //Draws the entity, as well as a green mask behind
   //it if ENTITY_MASKS is true
   virtual void draw(SDL_Surface *screen = screen_) const;
   
   //Any changes that need to be made to the entity
   //by the game loop
   virtual void tick(double delta);

   //Initializes the class' static pointer
   static void init(GameData *game, SDL_Surface *screen);

   //How much of the entity should be drawn.
   //FULL (1.0) by default
   virtual double getDrawPercent() const;

   //The color used to draw the entity
   //(See enum EntityColor)
   virtual int getColor() const;

   //identitifes which entity subclass this is
   virtual EntityTypeID classID() const = 0;

   //whether these entities can collide with others
   virtual bool collides() const;
};

#endif