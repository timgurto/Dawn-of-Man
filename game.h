// (C) 2009 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "Debug.h"
#include "GameData.h"

void addEntity(GameData &game, Entity *entity);
void gameMode();
void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *image, SDL_Surface *cursor,
                    const Point &mousePos, const GameData &game);
void updateState();

#endif