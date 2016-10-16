// (C) 2010 Tim Gurto

#ifndef SURFACE_H
#define SURFACE_H

#include <string>
#include "SDL.h"
#include "SDL_ttf.h"
#include "types.h"
#include "globals.h"

extern pixels_t SCREEN_WIDTH, SCREEN_HEIGHT;

enum SpecialSurface{
   SUR_UNINIT, //not yet a surface; needs to be set later with =
   SUR_SCREEN, //the screen buffer
   SUR_BLANK   //a blank 
};

//encapsulates graphics functionality
class Surface{

   //the actual surface pointer
   SDL_Surface *surface_;

   //whether this surface is the screen buffer;
   bool isScreen_;

   //Keeps track of allocated surfaces
   static int surfacesLoaded_;

   //whether there is a screen buffer
   static bool screenSet_;

public:

   Surface(const std::string fileName, bool alpha = false);
   Surface(const std::string fileName, const SDL_Color &background,
           bool alpha = false);
   Surface(SpecialSurface special = SUR_UNINIT,
           int width = SCREEN_WIDTH, int height = SCREEN_HEIGHT,
           SDL_Color background = NO_COLOR);
   Surface(TTF_Font *font, std::string message, SDL_Color color);
   Surface(const Surface &original);
   
   ~Surface();

   Surface &operator=(const Surface &rhs);
   operator bool() const;
   SDL_Surface *operator->();
   const SDL_Surface *operator->() const;

   //need to be called manually, in main or wherever
   static void init();
   static void quit();

   //load a surface from an image file
   void loadImage(const std::string &fileName, bool alpha);

   //set the surface's transparent color
   void setColorKey(const SDL_Color &color);

   //fills a part of the surface with color
   void fill(const SDL_Color color, SDL_Rect *rect = 0);
   void fill(Uint32 color = BLACK_UINT, SDL_Rect *rect = 0);

   //draw onto another surface
   void draw(Surface &dst,
             SDL_Rect *dstRect = 0, SDL_Rect *srcRect = 0) const;
   friend void operator<<(Surface &dst, const Surface &src);

   //sets the surface's alpha value (0-ff)
   void setAlpha(Uint8 alpha);

   //flips the screen buffer
   void flip();

   //saves the surface as an image file
   void saveToBitmap(std::string &fileName) const;
};

#endif