// (C) 2009 Tim Gurto

#ifndef UI_H
#define UI_H

#include <string>
#include "SDL.h"
#include "globals.h"

struct GameData;

class UI{
   SDL_Surface *topBar;
   SDL_Surface *bottomBar;
   pixels_t barWidth;

public:
   UI(const std::string &topBarPath,
      const std::string &bottomBarPath,
      const SDL_Color &background);
   ~UI();
   void draw(SDL_Surface *screen, const GameData &game,
      ControlMode controlmode) const;
};


#endif