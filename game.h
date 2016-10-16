// (C) 2009-2010 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "GameData.h"

class Entity;

//Main game functionality
void gameMode();

//Adds an entity to the list, signing it up for
//garbage collection upon removal
//Entity must have been allocated with new
void addEntity(GameData &game, Entity *entity);

//Initializes the game's base data
void initializeData(char *filename, GameData &game);

#endif