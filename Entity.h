// (C) 2009 Tim Gurto

#ifndef ENTITY_H
#define ENTITY_H

#include "SDL.h"
#include "Point.h"
#include "EntityType.h"
//#include "GameData.h"
struct GameData;

class Entity{
   friend class EntityPtr;
protected:
   typeNum_t type_;
   Point loc_;

public:
   Entity(typeNum_t type, const Point &loc);
   bool operator<(const Entity &rhs) const;
   const virtual EntityType &type(const GameData &game) const = 0;
   void draw(SDL_Surface *screen, const GameData &game) const;
};

#endif