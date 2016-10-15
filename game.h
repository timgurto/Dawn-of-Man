// (C) 2009 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "Debug.h"
#include "GameData.h"

void gameMode();
void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *image, SDL_Surface *cursor,
                    pixels_t mouseX, pixels_t mouseY,
                    const GameData &game);
void updateState();

#endif