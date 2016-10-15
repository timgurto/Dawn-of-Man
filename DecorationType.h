// (C) 2009-2010 Tim Gurto

#ifndef DECORATION_TYPE_H
#define DECORATION_TYPE_H

#include "EntityType.h"

//Entities which are not functional, and
//do not collide.
//Minimalist class
class DecorationType: public EntityType{
   friend class Decoration;

   //the sprites' color
   int color_;

   //whether this kind of decoration collides
   bool collides_;

public:
   DecorationType(typeNum_t index,
                  const std::string &name,
                  const SDL_Rect &drawRect,
                  const SDL_Rect &baseRect,
                  const Point &selectionCenter,
                  int color,
                  bool collides = false);
};

#endif