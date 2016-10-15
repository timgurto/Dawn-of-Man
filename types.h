// (C) 2010 Tim Gurto

#include "SDL.h"

//indexes for entity types, players, etc.
typedef unsigned short typeNum_t;

//TODO look at switching to double
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