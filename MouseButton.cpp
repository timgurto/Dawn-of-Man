// (C) 2010 Tim Gurto

#include "MouseButton.h"
#include "globals.h"
#include "Point.h"

const pixels_t MouseButton::MOVEMENT_THRESHOLD = 10;

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