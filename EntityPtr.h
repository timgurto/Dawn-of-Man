// (C) 2009 Tim Gurto

#ifndef ENTITY_PTR_H
#define ENTITY_PTR_H

#include "Entity.h"

class EntityPtr{
   Entity *ptr_;

public:
   EntityPtr(Entity *ptr);
   EntityPtr operator=(Entity *ptr);
   bool operator<(const EntityPtr &rhs) const;
   Entity *operator->();
   Entity &operator*();
};

#endif