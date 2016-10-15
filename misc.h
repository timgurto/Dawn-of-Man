#ifndef SDL_WRAPPERS_H
#define SDL_WRAPPERS_H

#include <string>

#include "SDL.h"

#include "globals.h"

//SDL
SDL_Surface *loadImage(const char* fileName);
SDL_Surface *loadImage(const char* fileName, SDL_Color background);
SDL_Surface *loadImage(const std::string fileName);
SDL_Surface *loadImage(const std::string fileName, SDL_Color background);
SDL_Rect makeRect(Sint16 x = 0, Sint16 y = 0, Uint16 w = 0, Uint16 h = 0);
SDL_Color makeColor(Uint8 r, Uint8 g, Uint8 b);
SDL_Surface *setScreen();
void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, SDL_MouseMotionEvent coords);
void freeSurface(SDL_Surface *&p);

//misc
std::string makePath(EntityType type, typeNum_t imageNumber, bool portrait = false);
template <typename Type> void checkP(Type *pointer);

#endif