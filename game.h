// (C) 2009-2010 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "globals.h"
#include "update.h"
#include "render.h"
#include "UIBar.h"
#include "GameData.h"
class Entity;

//Main game functionality
void gameMode();

//Adds an entity to the list, signing it up for garbage collection upon
//removal
void addEntity(GameData &game, Entity *entity);

//Removes an entity from the list, and deallocates it
void removeEntity(entities_t::iterator it);

#endif