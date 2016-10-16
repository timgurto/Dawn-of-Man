// (C) 2010 Tim Gurto

#ifndef SCREEN_H
#define SCREEN_H

#include "Point.h"

class Surface;

class Screen{
   
   static Surface
      *background_, //the background image for all screens
      *screen_,     //the screen buffer.  Confusing name...
      *cursor_;     //the cursor image

   static Point mousePos;

public:
   
   //display the screen, trap execution and handle input
   //until a button is pushed
   void go();

   //inspect SDL's event queue and deal accordingly
   void handleEvents();

   //draw the screen
   void draw();

   //initialize static pointers
   static void init(Surface *background,
                    Surface *screen,
                    Surface *cursor);
};

#endif