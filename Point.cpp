// (C) 2009-2010 Tim Gurto

#include "SDL.h"
#include "misc.h"

Point::Point(const SDL_Rect &original):
x(original.x),
y(original.y){}

Point::Point(pixels_t xCoord, pixels_t yCoord):
x(xCoord),
y(yCoord){}

Point Point::operator=(const SDL_Rect &rhs){
   Point p(rhs.x, rhs.y);
   return p;
}

Point Point::operator+(const Point &rhs) const{
   return Point(x + rhs.x, y + rhs.y);
}

Point Point::operator-(const Point &rhs) const{
   return Point(x - rhs.x, y - rhs.y);
}

SDL_Rect Point::operator-(const SDL_Rect &rhs) const{
   return makeRect(x-rhs.x, y-rhs.y, -1 * rhs.w, -1 * rhs.h);
}

SDL_Rect Point::operator+(const SDL_Rect &rhs) const{
   return makeRect(x+rhs.x, y+rhs.y, rhs.w, rhs.h);
}

SDL_Rect operator+(SDL_Rect lhs, const Point &rhs){
   lhs.x += rhs.x;
   lhs.y += rhs.y;
   return lhs;
}

SDL_Rect operator-(SDL_Rect lhs, const Point &rhs){
   lhs.x -= rhs.x;
   lhs.y -= rhs.y;
   return lhs;
}