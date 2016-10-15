// (C) 2009 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "globals.h"
#include "UIBar.h"
class Debug;
class Entity;
struct GameData;

//Main game functionality
void gameMode();

//Draws a frame
void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *cursor, SDL_Surface *cursorShadow,
                    SDL_Surface *entitiesTemp,
                    const Point &mousePos, const GameData &game,
                    const UIBars_t &bars, typeNum_t toBuild);

//Any processing within the game loop
void updateState(GameData &game);

//Adds an entity to the list, signing it up for garbage collection upon
//removal
void addEntity(GameData &game, Entity *entity);

//Removes an entity from the list, and deallocates it
void removeEntity();

//Draws the cursor to the screen
void blitCursor (SDL_Surface *cursor, SDL_Surface *shadow,
                 SDL_Surface *screen, const Point &coords);

#endif