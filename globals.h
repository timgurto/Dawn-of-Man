// (C) 2009-2010 Tim Gurto

#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include "SDL.h"
#include "Point.h"

//DEBUG determines:
// -The displaying of debug messages
#ifdef NDEBUG
const bool DEBUG = false;
#else
//TODO make "debug" a macro
const bool DEBUG = true;
#endif

typedef unsigned short typeNum_t;
typedef Sint32 pixels_t;
typedef Uint32 timer_t;
typedef double progress_t;

const typeNum_t MAX_TYPES = 256;
const typeNum_t MAX_PLAYERS = 2;

//higher = slower game speed.
const int DELTA_MODIFIER = 40;

//progress points added per state update
//progress points: proportional to completion time
const progress_t PROGRESS_PER_CALC = 4;

const pixels_t SCREEN_WIDTH = 1024;
const pixels_t SCREEN_HEIGHT = 768;
const int SCREEN_BPP = 32;

//in case the cursor point isn't at 0,0
const Point CURSOR_OFFSET(0,-9);
const Point CURSOR_RAISED(-16, 16);

const typeNum_t NO_TYPE = 0xffff;

//unfortunately, different SDL functions use different
//color formats
const SDL_Color BLACK   = {0, 0, 0};
const SDL_Color BLUE    = {0, 0, 0xff};
const SDL_Color GREEN   = {0, 0xff, 0};
const SDL_Color CYAN    = {0, 0xff, 0xff};
const SDL_Color RED     = {0xff, 0, 0};
const SDL_Color MAGENTA = {0xff, 0, 0xff};
const SDL_Color YELLOW  = {0xff, 0xff, 0};
const SDL_Color WHITE   = {0xff, 0xff, 0xff};
const Uint32 BLACK_UINT   = 0x000000;
const Uint32 BLUE_UINT    = 0x0000ff;
const Uint32 GREEN_UINT   = 0x00ff00;
const Uint32 CYAN_UINT    = 0x00ffff;
const Uint32 RED_UINT     = 0xff0000;
const Uint32 MAGENTA_UINT = 0xff00ff;
const Uint32 YELLOW_UINT  = 0xffff00;
const Uint32 WHITE_UINT   = 0xffffff;

const Uint8 MOUSE_BUTTON_LEFT        = 1;
const Uint8 MOUSE_BUTTON_MIDDLE      = 2;
const Uint8 MOUSE_BUTTON_RIGHT       = 3;
const Uint8 MOUSE_BUTTON_SCROLL_UP   = 4;
const Uint8 MOUSE_BUTTON_SCROLL_DOWN = 5;

const float FULL  = 1;
const float EMPTY = 0;

//whether particles fade over time
const bool PARTICLE_FADE = false;

//Alpha of shadows (particles, cursor)
const Uint8 SHADOW_ALPHA = 0x44;

//Graphics settings: each of these has a substantial
//(~45%, multiplicative) effect on performance
//===================================================
//anything behind each entity is cleared
const bool ENTITY_MASKS = true;
//entities appear engraved, cursor and particles have
//shadows
const bool SHADOWS = true;
//===================================================

const SDL_Color ENTITY_BACKGROUND   = GREEN;
const Uint32 ENTITY_BACKGROUND_UINT = GREEN_UINT;
const SDL_Color ENTITY_MASK         = MAGENTA;
const Uint32 ENTITY_MASK_UINT       = MAGENTA_UINT;

const Uint32 FOOTPRINT_COLOR_GOOD = 0x008800; //dark green
const Uint32 FOOTPRINT_COLOR_BAD  = 0x880000; //dark red

const pixels_t MAP_TILE_SIZE = 512;
const pixels_t SCROLL_AMOUNT = 40;
const pixels_t SCROLL_MARGIN = 50;

const pixels_t RMB_MOVEMENT_THRESHOLD = 10;
const double RMB_SCROLL_MULTIPLIER = 0.5;

const pixels_t ICON_SIZE = 48;
const std::string IMAGE_SUFFIX = ".png";
const std::string IMAGE_PATH = "Images/";
const std::string MISC_IMAGE_PATH
   = IMAGE_PATH + "Misc/";
const std::string BUILDINGS_IMAGE_PATH
   = IMAGE_PATH + "Buildings/";
const std::string DECORATIONS_IMAGE_PATH
   = IMAGE_PATH + "Decorations/";
const std::string INTERFACE_IMAGE_PATH
   = IMAGE_PATH + "Interface/";

enum Direction{
   UP,
   DOWN,
   LEFT,
   RIGHT
};

//For makePath()
enum ImageModifier{
   NONE,
   ICON,
   MASK
};

//For makePath()
enum EntityTypeID{
   BUILDING,
   DECORATION,
   MAX_ENTITY_TYPE //for array sizes
};

//Interface modes for the game
enum ControlMode{
   NORMAL_MODE,
   BUILD_MODE,
};

//Corners of the screen; for UIBar
enum Corner{
   TOP_LEFT,
   TOP_RIGHT,
   BOTTOM_LEFT,
   BOTTOM_RIGHT
};

//For UIBar
enum Orientation{
   VERTICAL,
   HORIZONTAL
};

//For the surface index
enum EntityColor{
   ENTITY_DEFAULT = MAX_PLAYERS,
   ENTITY_SHADOW_LIGHT,
   ENTITY_SHADOW_DARK,
   ENTITY_DECORATION,
   ENTITY_MAX //for array sizes
};
const Uint32 ENTITY_DEFAULT_COLOR = 0xaaaa22;
const Uint32 ENTITY_DECORATION_COLOR = 0x888888;

#endif