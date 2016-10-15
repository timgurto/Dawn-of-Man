#ifndef GAME_H
#define GAME_H

#include "Debug.h"

void gameMode();
void drawEverything(SDL_Surface *screen,
                    SDL_Surface *s, SDL_Surface *back,
                    SDL_Surface *image, SDL_Surface *cursor,
                    screenSize_t mouseX, screenSize_t mouseY);
void updateState();

#endif