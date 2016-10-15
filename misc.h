// (C) 2009 Tim Gurto

#ifndef MISC_H
#define MISC_H

#include <string>

#include "SDL.h"
#include "globals.h"

class Entity;
class EntityType;
struct Point;
struct GameData;

//SDL
SDL_Surface *loadImage(const char* fileName);
SDL_Surface *loadImage(const char* fileName, const SDL_Color &background);
SDL_Surface *loadImage(const std::string fileName);
SDL_Surface *loadImage(const std::string fileName, const SDL_Color &background);
SDL_Rect makeRect(Sint16 x = 0, Sint16 y = 0, Uint16 w = 0, Uint16 h = 0);
SDL_Rect makeRect(const Point &point);
SDL_Color makeColor(Uint8 r, Uint8 g, Uint8 b);
SDL_Surface *setScreen();
void freeSurface(SDL_Surface *&p);
SDL_Surface *copySurface(SDL_Surface* src);

//misc
std::string makePath(EntityTypeID type, typeNum_t imageNumber,
                     ImageModifier modifier = NONE);

template <typename Type> void checkP(Type *pointer){
   if (pointer == 0){
      SDL_WM_SetCaption(SDL_GetError(), NULL);
      bool pointerProperlyInitialized(false);
      assert (pointerProperlyInitialized);
   }
   assert (pointer != 0);
}

bool dereferenceLessThan(Entity *p1, Entity *p2);
bool pointCollision(const Point &point, const SDL_Rect rect);
SDL_Surface *getBuildingTypeIcons(typeNum_t i, const GameData &game);
bool noCollision(const GameData &game, const EntityType &type,
               const Point &mousePos);
bool collision(const SDL_Rect &r1, const SDL_Rect &r2);

#endif