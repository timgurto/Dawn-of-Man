// (C) 2010 Tim Gurto

#ifndef MOUSE_BUTTON_H
#define MOUSE_BUTTON_H

#include "types.h"
#include "Point.h"

//TODO comments
//Describes the state of a mouse button
struct MouseButton{

   //How far the mouse has to move to switch
   //from a click to a drag
   static const pixels_t MOVEMENT_THRESHOLD;
   
   //Where the cursor was when a drag action began
   Point dragBegin;

   //Whether the button is down
   bool down;

   //Whether the button is dragging
   bool dragging;

   MouseButton();

   //sets values when the button is clicked
   void mouseDown(const Point &mousePos);

   //sets values when the button is released
   void mouseUp();

   //checks whether the mouse has moved enough,
   //and if so flags the button as dragging
   void checkDrag(const Point &mousePos);
};

#endif