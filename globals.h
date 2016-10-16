// (C) 2009-2010 Tim Gurto

#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include "SDL.h"
#include "Point.h"
#include "types.h"

//DEBUG means:
// -The displaying of debug messages
// -The automatic display of delta/FPS
// -Automatic windowed mode
#ifdef NDEBUG //VS: Debug vs. Release mode
#define DEBUG false
#else
#define DEBUG true
#endif

const double PI = 3.14159265358979323846;

//a yet-to-be-calculated angle
const double DUMMY_ANGLE = 3 * PI;

const typeNum_t MAX_TYPES = 8;
const typeNum_t NO_TYPE = 0xffff;

const double DUMMY_PROGRESS = -1;

const typeNum_t MAX_PLAYERS = 3; //including Nature
const typeNum_t HUMAN_PLAYER = 1;
const typeNum_t NATURE_PLAYER = 0;

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

const SDL_Color ENTITY_BACKGROUND   = GREEN;
const Uint32 ENTITY_BACKGROUND_UINT = GREEN_UINT;
const SDL_Color ENTITY_MASK         = MAGENTA;
const Uint32 ENTITY_MASK_UINT       = MAGENTA_UINT;

//const Uint32 FOOTPRINT_COLOR_GOOD = 0x008800; //dark green
//const Uint32 FOOTPRINT_COLOR_BAD  = 0x880000; //dark red

//Alpha of shadows (particles, cursor)
const Uint8 SHADOW_ALPHA = 0x44;

//This has a substantial effect on performance
//anything behind each entity is cleared
const bool ENTITY_MASKS = true;

const double FULL  = 1;
const double EMPTY = 0;

const double PATH_CHECK_DISTANCE = 4;
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
const std::string TECHS_IMAGE_PATH
   = IMAGE_PATH + "Techs/";
const std::string INTERFACE_IMAGE_PATH
   = IMAGE_PATH + "Interface/";
const std::string SOUND_PATH = "Sounds/";
const std::string DATA_PATH = "Data/";

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
   ENT_DECORATION,
   ENT_RESOURCE_NODE,

   ENT_MAX, //for array sizes.
   
   //not strictly an entity, but used for makePath()
   ENT_TECH,

   ENT_NONE,
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
   CLR_SHADOW_DARK,
   CLR_SHADOW_LIGHT,
   CLR_BLACK,
   CLR_DECORATION,
   CLR_RESOURCE_FOOD,
   CLR_RESOURCE_WOOD,

   CLR_MAX //for array sizes
};
const Uint32 ENTITY_DEFAULT_COLOR = 0xaaaa22;

const SDL_Color ENGRAVE_DARK      = {0x44, 0x44, 0x44};
const Uint32 ENGRAVE_DARK_UINT    = 0x444444;
const SDL_Color ENGRAVE_LIGHT     = {0xbb, 0xbb, 0xbb}; //{0xdd, 0xdd, 0xdd};
const Uint32 ENGRAVE_LIGHT_UINT   = 0xbbbbbb; //0xdddddd;

const Uint32 RESOURCE_WOOD_COLOR  = 0x406040;
const Uint32 RESOURCE_FOOD_COLOR  = 0xd75757;

const Uint32 DECORATION_COLOR     = 0x666666;

const Uint8 BLACK_ENTITY_ALPHA = 0x3b; //0x44;
//The health percentage, below which the sprite is blackened
const double ENTITY_BLACK_HEALTH = 0.5;

//For re-sorting after movement
enum VerticalMovement{
   VM_UP,
   VM_DOWN,
   VM_NONE
};

#endif