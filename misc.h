// (C) 2009 Tim Gurto

#ifndef SDL_WRAPPERS_H
#define SDL_WRAPPERS_H

#include <string>

#include "SDL.h"
#include "globals.h"

class Entity;

//SDL
SDL_Surface *loadImage(const char* fileName);
SDL_Surface *loadImage(const char* fileName, const SDL_Color &background);
SDL_Surface *loadImage(const std::string fileName);
SDL_Surface *loadImage(const std::string fileName, const SDL_Color &background);
SDL_Rect makeRect(Sint16 x = 0, Sint16 y = 0, Uint16 w = 0, Uint16 h = 0);
SDL_Rect makeRect(const Point &point);
SDL_Color makeColor(Uint8 r, Uint8 g, Uint8 b);
SDL_Surface *setScreen();
void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, const Point &coords);
void freeSurface(SDL_Surface *&p);
SDL_Surface *copySurface(SDL_Surface* src);

//misc
std::string makePath(EntityTypeID type, typeNum_t imageNumber, bool portrait = false);

template <typename Type> void checkP(Type *pointer){
   if (pointer == 0){
      SDL_WM_SetCaption(SDL_GetError(), NULL);
      bool pointerProperlyInitialized(false);
      assert (pointerProperlyInitialized);
   }
   assert (pointer != 0);
}

bool dereferenceLessThan(Entity *p1, Entity *p2);

#endif