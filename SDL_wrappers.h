#ifndef SDL_WRAPPERS_H
#define SDL_WRAPPERS_H

#include <string>

#include "RGB.h"

SDL_Surface *loadImage(const char* fileName);
SDL_Surface *loadImage(const char* fileName, RGB background);
SDL_Surface *loadImage(const std::string fileName);
SDL_Surface *loadImage(const std::string fileName, RGB background);
SDL_Rect makeRect(Sint16 x = 0, Sint16 y = 0, Uint16 w = 0, Uint16 h = 0);
SDL_Surface *setScreen();
void checkP(SDL_Surface *p); //check pointer
void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, SDL_MouseMotionEvent coords);

#endif