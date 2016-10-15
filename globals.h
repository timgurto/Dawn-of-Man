#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include "SDL.h"

typedef short typeNum_t;
typedef int screenSize_t;
typedef Uint32 timer_t;

const bool DEBUG(true);

//Time between redraws/recalcs:
// true: X ms since last
// false: every X ms (catches up if there's a delay)
const bool MIN_WAIT = true;

const time_t DRAW_MS = 33; //time between redraws
const time_t CALC_MS = 1000; //time between state updates

const screenSize_t SCREEN_WIDTH(1024);
const screenSize_t SCREEN_HEIGHT(768);
const int SCREEN_BPP(32);

const Sint16 CURSOR_OFFSET_X(-2);
const Sint16 CURSOR_OFFSET_Y(-1);

const SDL_Color BLACK   = {0, 0, 0};
const SDL_Color BLUE    = {0, 0, 0xff};
const SDL_Color GREEN   = {0, 0xff, 0};
const SDL_Color CYAN    = {0, 0xff, 0xff};
const SDL_Color RED     = {0xff, 0, 0};
const SDL_Color MAGENTA = {0xff, 0, 0xff};
const SDL_Color YELLOW  = {0xff, 0xff, 0};
const SDL_Color WHITE   = {0xff, 0xff, 0xff};

const screenSize_t ICON_SIZE(48);

const std::string IMAGE_SUFFIX (".png");

const std::string IMAGE_PATH           ("Images/");
const std::string BUILDINGS_IMAGE_PATH ("Buildings/");

enum EntityType{
   Building
};

#endif