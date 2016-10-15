// (C) 2009 Tim Gurto

#ifndef UI_BAR_H
#define UI_BAR_H

#include "SDL.h"
#include "globals.h"

struct Point;
struct GameData;

//A bar of icons, which can be drawn on-screen and identify
//which of its buttons is clicked
class UIBar{
   Corner corner_; //corner of the screen where the bar begins
   Orientation orientation_; //vertical or horizontal

   //control mode necessary for this bar to appear
   ControlMode requiredMode_;

   //the function that returns icons to draw
   SDL_Surface *(*surfaceFun_)(typeNum_t index,
                               const GameData &game);

   //The rectangle describing the bar's dimensions
   SDL_Rect rect() const;

   //static pointers to access game data and bar backgrounds
   static const GameData *game_;
   static SDL_Surface *hBarSurface, vBarSurface;

public:
   UIBar(Corner corner, Orientation orientation,
         SDL_Surface *(*surfaceFun)(typeNum_t index,
                                    const GameData &game),
         typeNum_t initialIconCount,
         ControlMode requiredMode);

   //Initializes static pointers
   static void init(const GameData *game,
                    SDL_Surface *vBarSurface,
                    SDL_Surface *hBarSurface);

   //How many icons constitute the bar.  Note: public
   typeNum_t iconCount;

   //Draws the bar
   void draw(SDL_Surface *screen) const;

   //Which button the Point is pointing at;
   //NO_TYPE if none
   typeNum_t mouseIndex(const Point &point) const;
};

//So that all bars can be dealt with at the same time
typedef std::vector<UIBar *> UIBars_t;

#endif