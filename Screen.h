// (C) 2010 Tim Gurto

#ifndef SCREEN_H
#define SCREEN_H

#include "Point.h"

class Surface;

class Screen{

   typedef unsigned GoFun(Screen &thisScreen, const void *data);

   //What the screen does
   //Default: goDefault
   GoFun *go_;

   //whether to continue looping
   bool loop_;
   
   static Surface
      *background_, //the background image for all screens
      *cursor_;     //the cursor image

   static Point mousePos_;

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

   Screen(GoFun *go = &goDefault);

   //initialize static pointers
   static void init(Surface *background,
                    Surface *cursor);

   void operator()(const void *data = 0);
};

#endif