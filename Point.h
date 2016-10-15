// (C) 2009 Tim Gurto

#ifndef POINT_H
#define POINT_H

typedef Sint32 pixels_t;

struct Point{
   pixels_t x;
   pixels_t y;
   Point(const SDL_Rect &original);
   Point(pixels_t xCoord, pixels_t yCoord);
   //Point operator+(Point &rhs);
   //Point operator-(Point &rhs);
   SDL_Rect operator-(const SDL_Rect &rhs) const;
   SDL_Rect operator+(const SDL_Rect &rhs) const;

};

#endif