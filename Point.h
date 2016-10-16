// (C) 2009-2010 Tim Gurto

#ifndef POINT_H
#define POINT_H

#include "SDL.h"
#include "Types.h"

//Simple x and y, with operations to work with
//SDL_Rects
struct Point{
   pixels_t x;
   pixels_t y;

   Point(const SDL_Rect &original);
   Point(pixels_t xCoord = 0, pixels_t yCoord = 0);

   Point &operator=(const SDL_Rect &rhs);

   //addition/subtraction with other Points
   Point operator+(const Point &rhs) const;
   Point &operator+=(const Point &rhs);
   Point operator-(const Point &rhs) const;
   Point &operator-=(const Point &rhs);

   //scalar multiplication
   Point operator*(double rhs) const;

   //comparison
   bool operator<(const Point &rhs) const;
   bool operator==(const Point &rhs) const;
   bool operator!=(const Point &rhs) const;

   //addition/subtraction with SDL_Rects
   //width and height values of rhs are retained
   SDL_Rect operator-(const SDL_Rect &rhs) const;
   SDL_Rect operator+(const SDL_Rect &rhs) const;
};

//addition/subtraction with SDL_Rects, reversed
SDL_Rect operator+(SDL_Rect lhs, const Point &rhs);
SDL_Rect operator-(SDL_Rect lhs, const Point &rhs);

#endif