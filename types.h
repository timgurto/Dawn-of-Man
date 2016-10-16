// (C) 2010 Tim Gurto

#ifndef TYPES_H
#define TYPES_H

#include "SDL.h"
#include <queue>
#include <vector>

//indexes for entity types, players, etc.
typedef unsigned short typeNum_t;

//TODO consider switching to double
//screen locations and distances
typedef Sint32 pixels_t;

//millisecond measurements for the
//game loop delta
typedef Uint32 timer_t;

//construction/training progress.
//double, as progress is modified by delta
typedef double progress_t;

//attack, health, armor values in terms of
//entities' hitpoints
typedef unsigned short damage_t;

//the amount of a single resource type,
typedef unsigned resource_t;

//dealing with animation frames
typedef Uint8 frames_t;

//a set of points on the way to a unit's target
struct Point;
typedef std::queue<Point> path_t;

#endif