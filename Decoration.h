// (C) 2009 Tim Gurto

#ifndef DECORATION_H
#define DECORATION_H

#include "SDL.h"
#include "types.h"
#include "Entity.h"

struct GameData;
class EntityType;

//Entities which initially exist and are purely
//aesthetic
class Decoration : public Entity{

public:
   Decoration(typeNum_t type, const Point &loc);
   virtual const EntityType &Decoration::type() const;
   virtual EntityTypeID classID() const;
   virtual bool collides() const;
};


#endif