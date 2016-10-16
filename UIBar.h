// (C) 2009-2010 Tim Gurto

#ifndef UI_BAR_H
#define UI_BAR_H

#include <vector>
#include "globals.h"
#include "uiBarFunctions.h"

struct Point;
struct GameData;
class Surface;

//A bar of icons, which can be drawn on-screen and identify
//which of its buttons is clicked
class UIBar{
   Corner corner_; //corner of the screen where the bar begins
   Orientation orientation_; //vertical or horizontal

   //control mode necessary for this bar to appear
   ControlMode requiredMode_;

   //How many icons constitute the bar
   typeNum_t iconCount_;

   iconCountFunPtr iconCountFun_;

   //the function that returns icons to draw
   surfaceFunPtr surfaceFun_;

   //the function that executes when a button is pushed.
   clickFunPtr clickFun_;

   //The context help that appears when the bar is mouse-overed
   helpFunPtr helpFun_;

   //The rectangle describing the bar's dimensions
   SDL_Rect rect_;

   //static pointers to access game data and bar backgrounds
   static GameData *game_;
   static const CoreData *core_;
   static Surface *screen_;
   static Surface *barSurface_;
   static SDL_Sound *click_;

public:
   UIBar(Corner corner, Orientation orientation,
         iconCountFunPtr iconCountFun,
         surfaceFunPtr surfaceFun,
         clickFunPtr clickFun,
         helpFunPtr helpFun,
         ControlMode requiredMode);

   //Initializes static pointers
   static void init(const CoreData *core,
                    GameData *game,
                    Surface *screen,
                    Surface *barSurface,
                    SDL_Sound *click);

   //determines the number of icons on the bar, and
   //calculates the rectangle for drawing it
   void calculateRect();

   //Draws the bar
   void draw() const;

   //Which button the mouse is pointing at;
   //NO_TYPE if none
   typeNum_t mouseIndex() const;

   //Whether this bar is active, given the current game mode
   bool isActive();

   //executes the relevant action when the user clicks on
   //the bar; calls clickFun_
   void click(typeNum_t index = NO_TYPE);

   //returns the relevant context help text for the button
   std::string helpText(typeNum_t index);

   typeNum_t size() const;

   static void clickSound();
};

//So that all bars can be dealt with at the same time
typedef std::vector<UIBar *> UIBars_t;

#endif