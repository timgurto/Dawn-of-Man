#ifndef CAVE_H
#define CAVE_H

void caveMode();
void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *image, SDL_Surface *cursor,
                    screenSize_t mouseX, screenSize_t mouseY); 

#endif