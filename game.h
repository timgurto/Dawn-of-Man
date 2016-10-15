// (C) 2009 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "globals.h"
class Debug;
struct GameData;
class UI;

void gameMode();
void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *image, SDL_Surface *cursor,
                    const Point &mousePos, const GameData &game,
                    ControlMode controlMode, const UI &ui);
void updateState();
void addEntity(GameData &game, Entity *entity);
void removeEntity();
void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, const Point &coords);

#endif