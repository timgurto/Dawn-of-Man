// (C) 2009 Tim Gurto

#ifndef POINT_H
#define POINT_H

#include "globals.h"
#include "misc.h"

struct Point{
   pixels_t x;
   pixels_t y;

   Point(const SDL_Rect &original):
   x(original.x),
   y(original.y){}

   Point(pixels_t xCoord, pixels_t yCoord):
   x(xCoord),
   y(yCoord){}

   //Point operator+(Point &rhs){
   //   x += rhs.x;
   //   y += rhs.y
   //}

   //Point operator-(Point &rhs){
   //   x -= rhs.x;
   //   y -= rhs.y
   //}

   SDL_Rect operator-(const SDL_Rect &rhs) const{
      return makeRect(x-rhs.x, y-rhs.y);
   }

   SDL_Rect operator+(const SDL_Rect &rhs) const{
      return makeRect(x+rhs.x, y+rhs.y, rhs.w, rhs.h);
   }

};

#endif