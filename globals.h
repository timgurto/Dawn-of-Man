#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>

#include "RGB.h"

typedef short typeNum_t;
typedef Uint16 screenSize_t;

const bool DEBUG(true);

const screenSize_t SCREEN_WIDTH(1024);
const screenSize_t SCREEN_HEIGHT(768);
const int SCREEN_BPP(32);

const Sint16 CURSOR_OFFSET_X(-2);
const Sint16 CURSOR_OFFSET_Y(-1);

const RGB RGB_BLACK     (0, 0, 0);
const RGB RGB_BLUE      (0, 0, 0xff);
const RGB RGB_GREEN     (0, 0xff, 0);
const RGB RGB_CYAN      (0, 0xff, 0xff);
const RGB RGB_RED       (0xff, 0, 0);
const RGB RGB_MAGENTA   (0xff, 0, 0xff);
const RGB RGB_YELLOW    (0xff, 0xff, 0);
const RGB RGB_WHITE     (0xff, 0xff, 0xff);

const screenSize_t ICON_SIZE(48);

const std::string IMAGE_SUFFIX (".png");

const std::string IMAGE_PATH           ("Images/");
const std::string BUILDINGS_IMAGE_PATH ("Buildings/");

enum EntityType{
   Building
};

#endif