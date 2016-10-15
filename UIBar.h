// (C) 2009-2010 Tim Gurto

#ifndef UI_BAR_H
#define UI_BAR_H

#include "SDL.h"
#include "globals.h"
#include "uiBarFunctions.h"

struct Point;
struct GameData;

//A bar of icons, which can be drawn on-screen and identify
//which of its buttons is clicked
class UIBar{
   Corner corner_; //corner of the screen where the bar begins
   Orientation orientation_; //vertical or horizontal

   //control mode necessary for this bar to appear
   ControlMode requiredMode_;

   //How many icons constitute the bar
   typeNum_t iconCount;

   //the function that returns icons to draw
   SDL_Surface *(*surfaceFun_)(typeNum_t index,
                               const GameData &game);

   //The rectangle describing the bar's dimensions
   SDL_Rect rect;
   void calculateRect();

   //static pointers to access game data and bar backgrounds
   static GameData *game_;
   static SDL_Surface *screen_;
   static SDL_Surface *hBarSurface_, *vBarSurface_;

public:
   UIBar(Corner corner, Orientation orientation,
         SDL_Surface *(*surfaceFun)(typeNum_t index,
                                    const GameData &game),
         void (*clickFunction)(typeNum_t index,
                               GameData &game),
         typeNum_t initialIconCount,
         ControlMode requiredMode);

   //Initializes static pointers
   static void init(GameData *game,
                    SDL_Surface *screen,
                    SDL_Surface *vBarSurface,
                    SDL_Surface *hBarSurface);

   //Draws the bar
   void draw() const;

   //Which button the Point is pointing at;
   //NO_TYPE if none
   typeNum_t mouseIndex(const Point &point) const;

   //the function that executes when a button is pushed.
   void (*clickFun)(typeNum_t index, GameData &game);

   //Whether this bar is active, given the current game mode
   bool isActive(ControlMode mode);
};

//So that all bars can be dealt with at the same time
typedef std::vector<UIBar *> UIBars_t;

#endif