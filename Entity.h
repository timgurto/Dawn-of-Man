// (C) 2009 Tim Gurto

#ifndef ENTITY_H
#define ENTITY_H

#include "SDL.h"
#include "Point.h"
#include "globals.h"

struct GameData;
class EntityType;

class Entity{
   friend class EntityPtr;
protected:
   typeNum_t type_;
   Point loc_;
   static const GameData *game_;
   short direction;

public:
   Entity(typeNum_t type, const Point &loc);
   bool operator<(const Entity &rhs) const;
   const virtual EntityType &type() const = 0;
   void draw(SDL_Surface *screen) const;
   SDL_Rect baseRect();
   virtual void tick();
   static void setGame(GameData *game);
   virtual float getDrawPercent() const;
};

#endif