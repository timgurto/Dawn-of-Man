// (C) 2009 Tim Gurto
//TODO comments
#ifndef PARTICLE_H
#define PARTICLE_H

#include <cstdlib>
#include "SDL.h"
#include "globals.h"
#include "misc.h"

class Particle{

   double x, y; //position
   short age; //how long the particle has existed
   double hV, vV; //velocity
   double offset; //lateral displacement, for shadow

   static SDL_Surface *screen_, *image_, *shadow_;

public:

   //constants
   const static double GRAVITY;
   const static int PARTICLE_COUNT;
   const static double VELOCITY_RANGE;
   const static int DELAY; //ms
   const static int DECAY;

   Particle(const pixels_t xPos, const pixels_t yPos);

   void progress();

   void draw() const;

   bool expired() const;

   static void init(SDL_Surface *screen,
                    SDL_Surface *image,
                    SDL_Surface *shadow);
};

#endif