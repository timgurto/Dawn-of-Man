// (C) 2010 Tim Gurto

#ifndef UPDATE_H
#define UPDATE_H

#include "UIBar.h"
#include "GameData.h"
#include "MessageBox.h"

struct SDL_Surface;

//Any processing within the game loop
void updateState(double delta, const CoreData &core, GameData &game,
                 Surface &screen, UIBars_t &bars,
                 MessageBox &contextHelp,
                 MessageBox &resourcesBox, MessageBox &fpsDisplay);

//Handles events and changes game data accordingly
void handleEvents(const CoreData &core, GameData &game,
                  Surface &screen, UIBars_t &bars,
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

//Returns the address of the entity the mouse is currently over
Entity *findEntity(GameData &game, bool onlyTargetable = true);

//sets game mode based on what's selected
void setModeFromSelection(GameData &game, UIBars_t &bars);

//determines whether any player should be eliminated, and whether
//the human player won or lost.
void checkVictory(GameData &game);

//Adds an entity to the list, signing it up for
//garbage collection upon removal
//Entity must have been allocated with new
void addEntity(GameData &game, Entity *entity);

#endif