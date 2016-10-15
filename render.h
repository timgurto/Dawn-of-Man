// (C) 2010 Tim Gurto

#ifndef RENDER_H
#define RENDER_H

#include "UIBar.h"

struct GameData;
struct SDL_Surface;

//Draws everything to the screen
void render(SDL_Surface *screen, SDL_Surface *selection,
            SDL_Surface *diagGreen, SDL_Surface *diagRed,
            SDL_Surface *map, SDL_Surface *darkMap,
            SDL_Surface *cursor, SDL_Surface *cursorShadow,
            const GameData &game, const UIBars_t &bars);

//Draws the cursor
void renderCursor (SDL_Surface *screen, const GameData &game,
                   SDL_Surface *cursor, SDL_Surface *shadow);

//Draws the terrain and border tiles
void renderMap(SDL_Surface *screen, const GameData &game,
               SDL_Surface *map, SDL_Surface *border);

//Draws markers at the locations of selected entities
void renderSelection(SDL_Surface *screen, const GameData &game,
                     SDL_Surface *selection);

//Draws a building footprint at the cursor
void renderFootprint(SDL_Surface *screen, const GameData &game,
                     SDL_Surface *goodImage, SDL_Surface *badImage);

//Draws all entities
void renderEntities(SDL_Surface *screen, const GameData &game);

//Draws the selection rectangle
void renderSelectionRect(SDL_Surface *screen, const GameData &game);

//Draws all UIBars
void renderInterface(const UIBars_t &bars);

//Draws all active particles
void renderParticles(const GameData &game);

#endif