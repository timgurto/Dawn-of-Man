// (C) 2009-2010 Tim Gurto

#ifndef GAME_H
#define GAME_H

#include "GameData.h"

class Entity;
class Screen;

//Main game functionality
GameOutcome gameMode(Screen &thisScreen, const std::string *data);

#endif