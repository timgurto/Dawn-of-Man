// (C) 2010 Tim Gurto

#ifndef SCREEN_H
#define SCREEN_H

#include "Point.h"

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
   
   static Surface
      *background_, //the background image for all screens
      *cursor_;     //the cursor image

   //Default screen functionality.  Accepts input and renders
   //until a button is pushed.
   static GoFun goDefault;

   //inspect SDL's event queue and deal accordingly
   //used by goDefault
   void handleEventsDefault();

   //draw the screen
   //used by goDefault
   void drawDefault() const;

public:

   static Point mousePos;
   static Point screenRes;

   //Default screen resolutions - 16:10 16:9 and 4:3
   static const Point
      DEFAULT_SCREEN_3,
      DEFAULT_SCREEN_2,
      DEFAULT_SCREEN_1;
   
   static std::vector<Point> preferredResolutions;

   Screen(GoFun *go = &goDefault);

   //initialize static pointers
   static void init(Surface *background,
                    Surface *cursor);

   //sets screen resolution based on available settings and args
   static void setScreenResolution(int argc, char **argv);

   int operator()(const void *data = 0);
};

#endif