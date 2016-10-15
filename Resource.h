// (C) 2010 Tim Gurto

#ifndef RESOURCE_H
#define RESOURCE_H

#include "SDL.h"
#include "types.h"
#include "Entity.h"

struct GameData;
class EntityType;

//Entities which initially exist and are purely
//aesthetic
class Resource: public Entity{

public:
   Resource(typeNum_t type, const Point &loc);
   virtual const EntityType &/*Resource::*/type() const;
   virtual EntityTypeID classID() const;
};

#endif