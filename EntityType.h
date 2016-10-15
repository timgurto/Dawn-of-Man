// (C) 2009-2010 Tim Gurto

#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H

#include <string>
#include "SDL.h"
#include "Point.h"
#include "globals.h"

//A category of entities, differing by attributes,
//regular actions, and in-game behavior
class EntityType{
   friend class Entity;
   friend class Building;
   friend class Unit;

   //for loading graphics, and position in vector
   typeNum_t index_;

   std::string name_; //the entity's name

   SDL_Rect
      drawRect_, //position of sprite graphic
      baseRect_; //the entity's base, for collisions

   //where the selection indicator is centered
   Point selectionCenter_;

   SDL_Surface
      *surface_, //main image
      *icon_, //icon, for UIBars
      *mask_; //green and magenta background mask

public:
   //deep-copy, loading and allocating new surfaces
   EntityType(typeNum_t index, EntityTypeID type,
              const std::string &name,
              const SDL_Rect &drawRect,
              const SDL_Rect &baseRect,
              const Point &selectionCenter);
   EntityType(const EntityType &original);

   ~EntityType();

   //get
   const SDL_Rect &getBaseRect() const;
   const SDL_Rect &getDrawRect() const;
   SDL_Surface *getIcon() const;
   const std::string &getName();
};

#endif