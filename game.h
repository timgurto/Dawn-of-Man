// (C) 2009-2010 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "globals.h"
#include "UIBar.h"
#include "GameData.h"
class Entity;

//Main game functionality
void gameMode();



//=== update ===


//Any processing within the game loop
void updateState(double delta, GameData &game, SDL_Surface *screen,
                 UIBars_t &bars);

//Handles events and changes game data accordingly
void handleEvents(GameData &game, SDL_Surface *screen, UIBars_t &bars);

//Adds an entity to the list, signing it up for garbage collection upon
//removal
void addEntity(GameData &game, Entity *entity);

//Removes an entity from the list, and deallocates it
void removeEntity(entities_t::iterator it);

//Scrolls the map, based on keyboard/mouse states
void scrollMap(GameData &game, double Delta);

//calculates the dimensions of the selection rectangle,
//for drawing or determining collisions with entities
SDL_Rect getSelectionRect(const GameData &game);

//Selects entities chosen by the user
void select(GameData &game);

void setSelectedTargets(GameData &game);

void reSort(entities_t &entities, entities_t::iterator it,
            VerticalMovement verticalMovement);



//=== render ===


//Draws a frame
void render(SDL_Surface *screen, SDL_Surface *glow,
            SDL_Surface *diagGreen, SDL_Surface *diagRed,
            SDL_Surface *map, SDL_Surface *darkMap,
            SDL_Surface *cursor, SDL_Surface *cursorShadow,
            SDL_Surface *entitiesTemp,
            const GameData &game, const UIBars_t &bars);

//Draws the cursor to the screen
void blitCursor (SDL_Surface *cursor, SDL_Surface *shadow,
                 SDL_Surface *screen, const GameData &game);
#endif