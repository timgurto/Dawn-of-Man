// (C) 2010 Tim Gurto

#include <string>
#include <cassert>
#include "SDL_ttf.h"
#include "ScreenElement.h"
#include "Surface.h"
#include "Screen.h"
#include "util.h"

SDL_Color      ScreenElement::defaultLabelColor_ = WHITE;
SDL_Color      ScreenElement::defaultButtonColor_ = YELLOW;
SDL_Color      ScreenElement::defaultButtonAltColor_ = {0xca, 0x65, 0x0};
int            ScreenElement::defaultFontSize_ = 30;
std::string    ScreenElement::defaultFontName_ = FONT_GAME;
const int      ScreenElement::NO_ID = -1;

ScreenElement::ScreenElement(ScreenElementType type,
                             std::string text,
                             Anchor anchor,
                             Point offset,
                             int id,
                             Point dim,
                             const Surface *background,
                             int fontSize,
                             SDL_Color fontColor,
                             std::string fontName):
type_(type),
text_(text),
id_(id),
rollover_(true){

   //handle default values
   if (fontSize == 0)
      fontSize = defaultFontSize_;
   if (fontColor == NO_COLOR)
      fontColor = type == ELEM_LABEL ?
                  defaultLabelColor_ :
                  defaultButtonColor_;

   //text surfaces
   TTF_Font *font = TTF_OpenFont(fontName.c_str(), fontSize);
   Surface textSurface(font, text, fontColor);
   Surface altSurface(font, text, defaultButtonAltColor_);
   Surface textLight(font, text, ENGRAVE_LIGHT / 2);
   Surface textDark (font, text, ENGRAVE_DARK / 2);   
   TTF_CloseFont(font);

   //fix default dimensions
   if (dim == Point(0, 0))
      dim = Point(textSurface->w, textSurface->h);

   image_ = Surface(SUR_BLANK, dim.x + 2, dim.y + 2);
   image_.setColorKey(GREEN);
   image_.fill(GREEN);

   altImage_ = image_; //hope this works!

   if (background)
      image_ << *background;
   textLight  .draw(image_, &makeRect(2, 2));
   textDark   .draw(image_);
   textSurface.draw(image_, &makeRect(1, 1));

   //alternate rollover image for buttons
   if (type_ == ELEM_BUTTON){
      if (background)
         altImage_ << *background;
      textLight .draw(altImage_, &makeRect(2, 2));
      textDark  .draw(altImage_);
      altSurface.draw(altImage_, &makeRect(1, 1));
   }


   //loc_
   Point res = Screen::getScreenRes();
   pixels_t
      midX = (res.x - dim.x) / 2,
      midY = (res.y - dim.y) / 2,
      rightX = res.x - dim.x,
      bottomY = res.y - dim.y;
   switch(anchor){
      case ANCHOR_TOP:          loc_ = Point(midX,   0);       break;
      case ANCHOR_BOTTOM:       loc_ = Point(midX,   bottomY); break;
      case ANCHOR_LEFT:         loc_ = Point(0,      midY);    break;
      case ANCHOR_RIGHT:        loc_ = Point(rightX, midY);    break;
      case ANCHOR_TOP_RIGHT:    loc_ = Point(rightX, 0);       break;
      case ANCHOR_BOTTOM_RIGHT: loc_ = Point(rightX, bottomY); break;
      case ANCHOR_BOTTOM_LEFT:  loc_ = Point(0,      bottomY); break;
      case ANCHOR_TOP_LEFT:     loc_ = Point(0,      0);       break;
      case ANCHOR_CENTER:       loc_ = Point(midX,   midY);    break;
      default: assert(false);
   }
   loc_ += offset;
}

void ScreenElement::draw() const{
   if (type_ == ELEM_BUTTON && rollover_)
      altImage_.draw(screenBuf, &makeRect(loc_));
   else
      image_.draw(screenBuf, &makeRect(loc_));
}