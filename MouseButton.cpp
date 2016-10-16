// (C) 2010 Tim Gurto

#include "MouseButton.h"
#include "types.h"
#include "Point.h"
#include "Debug.h"
#include "util.h"

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
      if (distance(mousePos, dragBegin) >= MOVEMENT_THRESHOLD)
         dragging = true;
}