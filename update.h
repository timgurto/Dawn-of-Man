// (C) 2010 Tim Gurto

#ifndef UPDATE_H
#define UPDATE_H

#include "UIBar.h"
#include "GameData.h"

struct SDL_Surface;

//TODO comments

//Any processing within the game loop
void updateState(double delta, GameData &game, SDL_Surface *screen,
                 UIBars_t &bars);

//Handles events and changes game data accordingly
void handleEvents(GameData &game, SDL_Surface *screen, UIBars_t &bars);

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

#endif