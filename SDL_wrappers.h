#ifndef SDL_WRAPPERS_H
#define SDL_WRAPPERS_H

#include <string>

#include "SDL.h"

#include "RGB.h"
#include "globals.h"

SDL_Surface *loadImage(const char* fileName);
SDL_Surface *loadImage(const char* fileName, RGB background);
SDL_Surface *loadImage(const std::string fileName);
SDL_Surface *loadImage(const std::string fileName, RGB background);
SDL_Rect makeRect(Sint16 x = 0, Sint16 y = 0, Uint16 w = 0, Uint16 h = 0);
SDL_Color makeColor(Uint8 r, Uint8 g, Uint8 b);
SDL_Surface *setScreen();
template <typename Type> void checkP(Type *pointer);
void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, SDL_MouseMotionEvent coords);
void freeSurface(SDL_Surface *&p);
std::string makePath(EntityType type, typeNum_t imageNumber, bool portrait = false);

#endif