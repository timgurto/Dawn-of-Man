// (C) 2010 Tim Gurto

#ifndef UPDATE_H
#define UPDATE_H

#include "UIBar.h"
#include "GameData.h"
#include "MessageBox.h"

struct SDL_Surface;
class Unit;

//Any processing within the game loop
void updateState(double delta, const CoreData &core, GameData &game,
                 UIBars_t &bars, MessageBox &contextHelp,
                 MessageBox &resourcesBox, MessageBox &fpsDisplay);

//Handles events and changes game data accordingly
void handleEvents(const CoreData &core, GameData &game,
                  UIBars_t &bars,
                  MessageBox &contextHelp, MessageBox &fpsDisplay);

//Scrolls the map, based on keyboard/mouse states
void scrollMap(GameData &game, double Delta);

//Calculates the dimensions of the selection rectangle,
//for drawing or determining collisions with entities
SDL_Rect getSelectionRect(const GameData &game);

//Selects entities chosen by the user
void select(GameData &game);

//Set the 'selected' attribute of appropriate units
void setSelectedTargets(GameData &game);

//Re-sorts a unit which has moved up or down.
void reSort(entities_t &entities, entities_t::iterator it,
            VerticalMovement verticalMovement);

//Returns the address of the entity the mouse is currently over,
//and if a unit is specified, an entity relevant to that unit.
Entity *findEntity(GameData &game, const Unit *targetingUnit = 0);

//sets game mode based on what's selected
void setModeFromSelection(GameData &game, UIBars_t &bars);

//determines whether any player should be eliminated, and whether
//the human player won or lost.
void checkVictory(GameData &game);

#endif