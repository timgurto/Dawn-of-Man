// (C) 2009 Tim Gurto

#ifndef UI_BAR_H
#define UI_BAR_H

#include "SDL.h"
#include "globals.h"

struct Point;
struct GameData;

class UIBar{
   Corner corner_;
   Orientation orientation_;
   ControlMode requiredMode_;
   //the function that returns icons to draw
   SDL_Surface *(*surfaceFun_)(typeNum_t index,
                               const GameData &game);
   SDL_Rect rect() const;

   static const GameData *game_;
   static const ControlMode *mode_;

public:
   UIBar(Corner corner, Orientation orientation,
         SDL_Surface *(*surfaceFun)(typeNum_t index,
                                    const GameData &game),
         typeNum_t initialIconCount,
         ControlMode requiredMode);
   static void set(const GameData *game, const ControlMode *mode);
   typeNum_t iconCount;
   void draw(SDL_Surface *screen) const;
   //which button the point is pointing at
   typeNum_t mouseIndex(const Point &point) const;
};

typedef std::vector<UIBar *> UIBars_t;

#endif