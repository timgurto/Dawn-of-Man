// (C) 2009 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "globals.h"
#include "UIBar.h"
class Debug;
struct GameData;

void gameMode();
void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *cursor, const Point &mousePos,
                    const GameData &game, const UIBars_t &bars);
void updateState();
void addEntity(GameData &game, Entity *entity);
void removeEntity();
void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, const Point &coords);

#endif