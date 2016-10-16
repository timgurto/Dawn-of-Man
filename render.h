// (C) 2010 Tim Gurto

#ifndef RENDER_H
#define RENDER_H

#include "UIBar.h"
#include "MessageBox.h"

struct CoreData;
struct GameData;
struct SDL_Surface;

//Draws the loading screen
void renderLoadingScreen(Surface &screen, const Surface &loading);

//Draws everything to the screen
void render(Surface &selection,
            Surface &diagGreen, Surface &diagRed,
            Surface &map, Surface &darkMap,
            Surface &cursor, Surface &cursorShadow,
            Surface &cursorPause, Surface &cursorColor,
            Surface &selRectImage,
            Surface *cursorIndex,
            const CoreData &core, const GameData &game,
            const UIBars_t &bars, const messageBoxes_t &messageBoxes,
            Surface outcomeMessage = Surface(SUR_UNINIT));

//Draws the cursor
void renderCursor (const GameData &game,
                   Surface &cursor, Surface &shadow,
                   Surface &pause, Surface &color,
                   Surface *cursorIndex);

//Draws the terrain and border tiles
void renderMap(const GameData &game,
               Surface &map, Surface &border);

//Draws markers at the locations of selected entities
void renderSelection(const GameData &game,
                     Surface &selection);

//Draws a building footprint at the cursor
void renderFootprint(const CoreData &core, const GameData &game,
                     Surface &goodImage, Surface &badImage);

//Draws all entities
void renderEntities(const GameData &game);

//Draws the selection rectangle
void renderSelectionRect(const GameData &game,
                         Surface &selRectImage);

//Draws all UIBars
void renderInterface(const UIBars_t &bars);

//Draws all active particles
void renderParticles(const GameData &game);

//Draws all message boxes
void renderMessageBoxes(const messageBoxes_t messageBoxes);

#endif