#include <cassert>

#include "SDL.h"
#include "SDL_wrappers.h"

#include "globals.h"
#include "cave.h"

void caveMode(){
   SDL_Surface *screen = setScreen();
   SDL_Surface *back = loadImage(IMAGE_PATH + "back.png");
   SDL_Surface *image = loadImage("Images/c0.png", RGB_MAGENTA);
   SDL_Surface *cursor = loadImage(IMAGE_PATH + "cursor.png", RGB_GREEN);

   screenSize_t
      mouseX(SCREEN_WIDTH/2),
      mouseY(SCREEN_HEIGHT/2);

   SDL_ShowCursor(SDL_DISABLE);

   
   
   typeNum_t toBuild = 0;

   bool loop = true;
   while (loop){
      //While there's an event to handle
      SDL_Event event;
      while (SDL_PollEvent(&event) ){
         switch (event.type){
            case SDL_MOUSEMOTION:
               mouseX = event.motion.x;
               mouseY = event.motion.y;
               break;
            case SDL_QUIT: //If the user has Xed out the window
               loop = false;
               break;
         }
      }
      drawEverything(screen, back, image, cursor, mouseX, mouseY);
   }

   SDL_ShowCursor(SDL_ENABLE);

   SDL_FreeSurface(image);

}

void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *image, SDL_Surface *cursor,
                    screenSize_t mouseX, screenSize_t mouseY){
   SDL_FillRect(screen, 0, 0);
   SDL_BlitSurface(back, 0, screen, &makeRect());
   SDL_BlitSurface(image, 0, screen, &makeRect(50,50));
   SDL_BlitSurface(cursor, 0, screen, &makeRect(
      CURSOR_OFFSET_X + mouseX, CURSOR_OFFSET_Y + mouseY));
   assert (SDL_Flip(screen) == 0);
}