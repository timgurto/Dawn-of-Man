// (C) 2010 Tim Gurto

#ifndef SCREEN_H
#define SCREEN_H

#include <string>
#include <vector>
#include "SDL_ttf.h"
#include "Point.h"
#include "ScreenElement.h"

class Surface;

class Screen{

public:
   typedef unsigned GoFun(Screen &thisScreen, const void *data);

private:

   //What the screen does
   //Default: goDefault
   GoFun *go_;

   //whether to continue looping
   bool loop_;

   //the on-screen elements
   elements_t elements_;

   //Default screen resolutions - 16:10 16:9 and 4:3
   static const Point defaultRes_[];
   
   static Surface
      *background_, //the background image for all screens
      *cursor_;     //the cursor image

   //Default screen functionality.  Accepts input and renders
   //until a button is pushed.
   static GoFun goDefault_;

   static Point screenRes_;
   static bool windowedMode_;

   //inspect SDL's event queue and deal accordingly
   //return the ID of any button pushed
   //used by goDefault
   int handleEventsDefault_();

   //draw the screen
   //used by goDefault
   void drawDefault_() const;

public:

   //nice and global, so everyone can keep it up to date.
   static Point mousePos;

   Screen(GoFun *go = &goDefault_);

   int operator()(const void *data = 0);

   void addElement(const ScreenElement &element);

   //initialize static pointers
   static void init(Surface *background,
                    Surface *cursor);

   //sets screen resolution based on available settings and args
   static void setScreenResolution(int argc, char **argv);

   static const Point &getScreenRes();
   static bool getWindowedMode();
};

#endif