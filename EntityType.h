// (C) 2009-2010 Tim Gurto

#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H

#include <string>
#include "SDL.h"
#include "globals.h"

//A category of entities, differing by attributes,
//regular actions, and in-game behavior
class EntityType{
   friend class Entity;
   friend class Building;

   typeNum_t index_; //for graphics
   std::string name_; //the entity's name
   SDL_Rect
      drawRect_, //position of sprite graphic
      baseRect_; //the entity's base, for collisions
   SDL_Surface
      *surface, //main image
      *icon, //icon, for UIBars
      *mask; //green and magenta background mask

public:
   EntityType(typeNum_t index, EntityTypeID type,
              const std::string &name,
              const SDL_Rect &drawRect,
              const SDL_Rect &baseRect);
   EntityType(const EntityType &original);
   EntityType &operator=(const EntityType &rhs);
   ~EntityType();
   SDL_Rect getBaseRect() const;
   SDL_Rect getDrawRect() const;
   SDL_Surface *getIcon() const;
   const std::string &getName();
};

#endif