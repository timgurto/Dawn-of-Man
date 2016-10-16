// (C) 2009, 2010 Tim Gurto

#ifndef DECORATION_H
#define DECORATION_H

#include "types.h"
#include "Entity.h"

struct GameData;
class EntityType;

//Entities which initially exist and are purely
//aesthetic
class Decoration : public Entity{

public:
   Decoration(typeNum_t type, const Point &loc);
   virtual const EntityType &type() const;
   virtual EntityTypeID classID() const;
   virtual bool collides() const;
   virtual int getColor() const;
};


#endif