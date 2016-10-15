// (C) 2009-2010 Tim Gurto

#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include "SDL.h"
#include "Point.h"
#include "types.h"

//DEBUG determines:
// -The displaying of debug messages
// -The displaying of delta/FPS
#ifdef NDEBUG //VS: Debug vs. Release mode
#define DEBUG false
#else
#define DEBUG true
#endif

const double PI = 3.14159265358979323846;

const typeNum_t MAX_TYPES = 256;
const typeNum_t NO_TYPE = 0xffff;

const typeNum_t MAX_PLAYERS = 2;
const typeNum_t HUMAN_PLAYER = 0;

//in case the cursor point isn't at 0,0
const Point CURSOR_OFFSET(0,-9);

//offset to draw raised cursor/shadow
const Point CURSOR_RAISED(-16, 16);

//bits per pixel
const int SCREEN_BPP = 32;


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
const SDL_Color GRAY    = {0x7f, 0x7f, 0x7f};
const Uint32 BLACK_UINT   = 0x000000;
const Uint32 BLUE_UINT    = 0x0000ff;
const Uint32 GREEN_UINT   = 0x00ff00;
const Uint32 CYAN_UINT    = 0x00ffff;
const Uint32 RED_UINT     = 0xff0000;
const Uint32 MAGENTA_UINT = 0xff00ff;
const Uint32 YELLOW_UINT  = 0xffff00;
const Uint32 WHITE_UINT   = 0xffffff;

const double FULL  = 1;
const double EMPTY = 0;

//Alpha of shadows (particles, cursor)
const Uint8 SHADOW_ALPHA = 0x44;

//This has a substantial effect on performance
//===================================================
//anything behind each entity is cleared
const bool ENTITY_MASKS = true;
//===================================================

const SDL_Color ENTITY_BACKGROUND   = GREEN;
const Uint32 ENTITY_BACKGROUND_UINT = GREEN_UINT;
const SDL_Color ENTITY_MASK         = MAGENTA;
const Uint32 ENTITY_MASK_UINT       = MAGENTA_UINT;

const Uint32 FOOTPRINT_COLOR_GOOD = 0x008800; //dark green
const Uint32 FOOTPRINT_COLOR_BAD  = 0x880000; //dark red

const SDL_Color ENGRAVE_DARK    = {0x44, 0x44, 0x44};
const Uint32 ENGRAVE_DARK_UINT  = 0x444444;
const SDL_Color ENGRAVE_LIGHT   = {0xdd, 0xdd, 0xdd};
const Uint32 ENGRAVE_LIGHT_UINT = 0xdddddd;

const pixels_t MAP_TILE_SIZE = 512;
const pixels_t SCROLL_AMOUNT = 40;
const pixels_t SCROLL_MARGIN = 100;
const pixels_t EDGE_SCROLL_MARGIN = 4;

const double RMB_SCROLL_MULTIPLIER = 0.3;

const pixels_t ICON_SIZE = 48;
const std::string IMAGE_SUFFIX = ".png";
const std::string IMAGE_PATH = "Images/";
const std::string MISC_IMAGE_PATH
   = IMAGE_PATH + "Misc/";
const std::string BUILDINGS_IMAGE_PATH
   = IMAGE_PATH + "Buildings/";
const std::string DECORATIONS_IMAGE_PATH
   = IMAGE_PATH + "Decorations/";
const std::string UNITS_IMAGE_PATH
   = IMAGE_PATH + "Units/";
const std::string RESOURCE_NODES_IMAGE_PATH
   = IMAGE_PATH + "Resources/";
const std::string INTERFACE_IMAGE_PATH
   = IMAGE_PATH + "Interface/";

enum Direction{
   DIR_UP,
   DIR_DOWN,
   DIR_LEFT,
   DIR_RIGHT,
   DIR_MAX
};

//For makePath()
enum ImageModifier{
   IMG_NONE,
   IMG_ICON,
   IMG_MASK
};

//For makePath()
enum EntityTypeID{
   ENT_BUILDING,
   ENT_UNIT,

   //Everything above this comment
   //has multiple colors.
   ENT_MAX, //for array sizes.
   ENT_NONE,

   ENT_DECORATION,
   ENT_RESOURCE_NODE
};

//Interface modes for the game
enum ControlMode{
   MODE_NORMAL,
   MODE_BUILDER,
   MODE_CONSTRUCTION, //a building is being placed
   MODE_BUILDING //a building is selected
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
   CLR_DEFAULT = MAX_PLAYERS,
   CLR_SAMPLE_0,
   //insert more colors here
   CLR_MAX //for array sizes
};
const Uint32 ENTITY_DEFAULT_COLOR = 0xaaaa22;

//For re-sorting after movement
enum VerticalMovement{
   VM_UP,
   VM_DOWN,
   VM_NONE
};

#endif