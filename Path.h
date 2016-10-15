// (C) 2010 Tim Gurto

#ifndef PATH_H
#define PATH_H

#include <queue>
#include "Point.h"

class Path{
   static const double ANGLE_NOT_CALCULATED;

   double angle;
   std::queue<Point> points;
};

#endif