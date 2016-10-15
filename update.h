// (C) 2010 Tim Gurto

#ifndef UPDATE_H
#define UPDATE_H

#include "UIBar.h"
#include "GameData.h"
#include "MessageBox.h"

struct SDL_Surface;

//Any processing within the game loop
void updateState(double delta, GameData &game, SDL_Surface *screen,
                 UIBars_t &bars, MessageBox &contextHelp,
                 MessageBox &resourcesBox, MessageBox &fpsDisplay,
                 Mix_Music *music);

//Handles events and changes game data accordingly
void handleEvents(GameData &game, SDL_Surface *screen,
                  UIBars_t &bars, MessageBox &contextHelp,
                  MessageBox &fpsDisplay, Mix_Music *music);

//Scrolls the map, based on keyboard/mouse states
void scrollMap(GameData &game, double Delta);

//Calculates the dimensions of the selection rectangle,
//for drawing or determining collisions with entities
SDL_Rect getSelectionRect(const GameData &game);

//Selects entities chosen by the user
void select(GameData &game, UIBars_t &bars);

//Set the 'selected' attribute of appropriate units
void setSelectedTargets(GameData &game);

//Re-sorts a unit which has moved up or down.
void reSort(entities_t &entities, entities_t::iterator it,
            VerticalMovement verticalMovement);

//Returns the address of the entity the mouse is currently over
Entity *findEntity(GameData &game);

#endif