// (C) 2010 Tim Gurto

#ifndef RENDER_H
#define RENDER_H

#include "UIBar.h"

struct GameData;
struct SDL_Surface;

//Draws everything to the screen
void render(SDL_Surface *screen, SDL_Surface *glow,
            SDL_Surface *diagGreen, SDL_Surface *diagRed,
            SDL_Surface *map, SDL_Surface *darkMap,
            SDL_Surface *cursor, SDL_Surface *cursorShadow,
            SDL_Surface *entitiesTemp,
            const GameData &game, const UIBars_t &bars);

//Draws the cursor
void renderCursor (SDL_Surface *screen, const GameData &game,
                   SDL_Surface *cursor, SDL_Surface *shadow);

#endif