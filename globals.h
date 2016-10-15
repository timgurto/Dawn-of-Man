// (C) 2009 Tim Gurto

#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include "SDL.h"
#include "Point.h"

#ifdef NDEBUG
const bool DEBUG = false;
#else
const bool DEBUG = true;
#endif

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
//progress points: proportional to completion time
const progress_t PROGRESS_PER_CALC = 10;

const pixels_t SCREEN_WIDTH = 1024;
const pixels_t SCREEN_HEIGHT = 768;
const int SCREEN_BPP = 32;

const pixels_t UI_BAR_PADDING = 24;

//in case the cursor point isn't at 0,0
const Point CURSOR_OFFSET(-2,-1);

const typeNum_t NO_TYPE = 0xffff;

const SDL_Color BLACK   = {0, 0, 0};
const SDL_Color BLUE    = {0, 0, 0xff};
const SDL_Color GREEN   = {0, 0xff, 0};
const SDL_Color CYAN    = {0, 0xff, 0xff};
const SDL_Color RED     = {0xff, 0, 0};
const SDL_Color MAGENTA = {0xff, 0, 0xff};
const SDL_Color YELLOW  = {0xff, 0xff, 0};
const SDL_Color WHITE   = {0xff, 0xff, 0xff};

const float FULL  = 1;
const float EMPTY = 0;
//anything behind each entity is cleared
const bool ENTITY_MASKS = true;
//whether the entire mask is drawn behind partial sprites
const bool MASK_BEFORE_CLIP = false;
const SDL_Color ENTITY_BACKGROUND = GREEN;
const SDL_Color ENTITY_MASK       = MAGENTA;
const Uint32 FOOTPRINT_COLOR_GOOD = 0x008800;
const Uint32 FOOTPRINT_COLOR_BAD  = 0x880000;

const pixels_t ICON_SIZE = 48;
const std::string IMAGE_SUFFIX = ".png";
const std::string IMAGE_PATH           = "Images/";
const std::string BUILDINGS_IMAGE_PATH = "Buildings/";
const std::string INTERFACE_IMAGE_PATH = "Interface/";

enum ImageModifier{
   NONE,
   ICON,
   MASK
};

enum EntityTypeID{
   BUILDING
};

enum ControlMode{
   NORMAL_MODE,
   BUILD_MODE
};

enum Corner{
   TOP_LEFT,
   TOP_RIGHT,
   BOTTOM_LEFT,
   BOTTOM_RIGHT
};

enum Orientation{
   VERTICAL,
   HORIZONTAL
};

#endif