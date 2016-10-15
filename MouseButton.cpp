// (C) 2010 Tim Gurto

#include "MouseButton.h"
#include "types.h"
#include "Point.h"
#include "Debug.h"

extern Debug debug;

const pixels_t MouseButton::MOVEMENT_THRESHOLD = 15;

MouseButton::MouseButton():
dragBegin(),
down(false),
dragging(false){}

void MouseButton::mouseDown(const Point &mousePos){
   down = true;
   dragging = false;
   dragBegin = mousePos;
}

void MouseButton::mouseUp(){
   down = false;
   dragging = false;
}

void MouseButton::checkDrag(const Point &mousePos){
   if (down && !dragging)
      if (mousePos.x - dragBegin.x >= MOVEMENT_THRESHOLD ||
          dragBegin.x - mousePos.x >= MOVEMENT_THRESHOLD ||
          mousePos.y - dragBegin.y >= MOVEMENT_THRESHOLD ||
          dragBegin.y - mousePos.y >= MOVEMENT_THRESHOLD){
         //debug("Dragging");
         dragging = true;
      }
}