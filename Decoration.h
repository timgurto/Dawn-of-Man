// (C) 2009 Tim Gurto

#ifndef DECORATION_H
#define DECORATION_H

#include "SDL.h"
#include "globals.h"
#include "Entity.h"

struct GameData;
class EntityType;

//Entities which initially exist and are purely
//aesthetic
class Decoration : public Entity{

public:
   Decoration(typeNum_t type, const Point &loc);
   const virtual EntityType &Decoration::type() const;
   virtual int getColor() const;
   virtual EntityTypeID classID() const;
};


#endif