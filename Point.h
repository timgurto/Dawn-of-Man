// (C) 2009 Tim Gurto

#ifndef POINT_H
#define POINT_H

typedef Sint32 pixels_t;

//Simple x and y, with operations to work with
//SDL_Rects
struct Point{
   pixels_t x;
   pixels_t y;
   Point(const SDL_Rect &original);
   Point(pixels_t xCoord, pixels_t yCoord);
   Point operator+(const Point &rhs) const;
   Point operator-(const Point &rhs) const;

   //width and height valyes of rhs are retained
   SDL_Rect operator-(const SDL_Rect &rhs) const;
   SDL_Rect operator+(const SDL_Rect &rhs) const;

};

#endif