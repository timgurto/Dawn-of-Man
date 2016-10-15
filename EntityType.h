// (C) 2009 Tim Gurto

#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H

#include <string>
#include "SDL.h"
#include "globals.h"

class EntityType{
   friend class Entity;

   typeNum_t index_;
   std::string name_;
   SDL_Rect drawRect_, baseRect_;
   SDL_Surface *surface, *icon, *mask;

public:
   EntityType(typeNum_t index, EntityTypeID type,
              const std::string &name,
              const SDL_Rect &drawRect,
              const SDL_Rect &baseRect);
   EntityType(const EntityType &original);
   EntityType &operator=(const EntityType &rhs);
   ~EntityType();
   SDL_Rect getBaseRect() const;
   SDL_Surface *getIcon() const;
};

#endif