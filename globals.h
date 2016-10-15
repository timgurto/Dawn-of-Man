// (C) 2009 Tim Gurto

#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include "SDL.h"
#include "Point.h"

//const bool DEBUG(true);
#ifdef NDEBUG
const bool DEBUG(false);
#else
const bool DEBUG(true);
#endif

//TODO move typedefs to separate file
typedef unsigned short typeNum_t;
typedef Sint32 pixels_t;
typedef Uint32 timer_t;
typedef Uint16 progress_t;


// true: X ms since last
// false: every X ms (catches up if there's a delay)
const bool MIN_WAIT = true;
const timer_t DRAW_MS = 1; //time between redraws
const timer_t CALC_MS = 40; //time between state updates
//progress points added per state update
//progress points = time to complete something
const progress_t PROGRESS_PER_CALC = 16;

const pixels_t SCREEN_WIDTH(1024);
const pixels_t SCREEN_HEIGHT(768);
const int SCREEN_BPP(32);

//whether graphics are outline-style
const bool OUTLINE_MODE(false);

const Point CURSOR_OFFSET(-2,-1);

const SDL_Color BLACK   = {0, 0, 0};
const SDL_Color BLUE    = {0, 0, 0xff};
const SDL_Color GREEN   = {0, 0xff, 0};
const SDL_Color CYAN    = {0, 0xff, 0xff};
const SDL_Color RED     = {0xff, 0, 0};
const SDL_Color MAGENTA = {0xff, 0, 0xff};
const SDL_Color YELLOW  = {0xff, 0xff, 0};
const SDL_Color WHITE   = {0xff, 0xff, 0xff};

const pixels_t ICON_SIZE(48);
const std::string IMAGE_SUFFIX (".png");
const std::string IMAGE_PATH           ("Images/");
const std::string BUILDINGS_IMAGE_PATH ("Buildings/");

enum EntityTypeID{
   BUILDING
};

#endif