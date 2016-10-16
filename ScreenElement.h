// (C) 2010 Tim Gurto

#ifndef SCREEN_ELEMENT_H
#define SCREEN_ELEMENT_H

#include <string>
#include "Point.h"
#include "Surface.h"

enum Anchor{
   ANCHOR_TOP,
   ANCHOR_BOTTOM,
   ANCHOR_LEFT,
   ANCHOR_RIGHT,
   ANCHOR_TOP_RIGHT,
   ANCHOR_BOTTOM_RIGHT,
   ANCHOR_BOTTOM_LEFT,
   ANCHOR_TOP_LEFT,
   ANCHOR_CENTER
};

enum ScreenElementType{
   ELEM_LABEL,
   ELEM_BUTTON //can be cicked to return its id_
};

class ScreenElement{
   friend class Screen;

   //The identifier to return if this element is activated
   //(eg. a button click)
   int id_;

   //location of the element
   Point loc_;
   
   //the element's type (button, label, etc.)
   ScreenElementType type_;

   //element's complete image
   Surface image_;

   //the text that appears on the element
   std::string text_;

   static SDL_Color defaultLabelColor_;
   static SDL_Color defaultButtonColor_;
   static int defaultFontSize_;
   static std::string defaultFontName_;

public:
   static const int NO_ID;

   ScreenElement(ScreenElementType type,
                 std::string text = "",

                 //which corner the element is anchored to
                 Anchor anchor = ANCHOR_CENTER,
                 //offset from the anchor
                 Point offset = Point(),

                 int id = NO_ID,
                 Point dim = Point(0, 0),
                 const Surface *background = 0,
                 int fontSize = defaultFontSize_,
                 SDL_Color fontColor = NO_COLOR, //use default for type
                 std::string fontName = defaultFontName_);

   void draw() const;
};

#endif