// (C) 2009-2010 Tim Gurto

#ifndef DECORATION_TYPE_H
#define DECORATION_TYPE_H

#include "EntityType.h"

//Entities which are not functional, and
//do not collide.
//Minimalist class
class DecorationType: public EntityType{
   friend class Decoration;

   //whether this kind of decoration collides
   bool collides_;

public:
   DecorationType(typeNum_t index,
                  const std::string &name,
                  const SDL_Rect &drawRect,
                  const SDL_Rect &baseRect,
                  bool collides = false);
};

#endif