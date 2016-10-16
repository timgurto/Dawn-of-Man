// (C) 2010 Tim Gurto

#include <string>
#include <cassert>
#include "SDL_ttf.h"
#include "ScreenElement.h"
#include "Surface.h"
#include "Screen.h"
#include "misc.h"

SDL_Color      ScreenElement::defaultLabelColor_ = WHITE;
SDL_Color      ScreenElement::defaultButtonColor_ = YELLOW;
int            ScreenElement::defaultFontSize_ = 30;
std::string    ScreenElement::defaultFontName_ = "Thor.ttf";
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
id_(id){

   //image_
   //TODO engraving... even though it's game-specific.

   //handle default values
   if (fontSize == 0)
      fontSize = defaultFontSize_;
   if (fontColor == NO_COLOR)
      fontColor = type == ELEM_LABEL ?
                  defaultLabelColor_ :
                  defaultButtonColor_;

   TTF_Font *font = TTF_OpenFont(fontName.c_str(), fontSize);
   Surface textSurface(font, text, fontColor);
   if (dim == Point(0, 0))
      dim = Point(textSurface->w, textSurface->h);
   image_ = Surface(SUR_BLANK, dim.x, dim.y);
   if (background){
      image_ << *background << textSurface;
   }else
      image_ = textSurface;

   TTF_CloseFont(font);


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
   image_.draw(screenBuf, &makeRect(loc_));
}