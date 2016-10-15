// (C) 2009 Tim Gurto
//TODO comments
#ifndef PARTICLE_H
#define PARTICLE_H

#include <cstdlib>
#include "SDL.h"
#include "globals.h"
#include "misc.h"

class Particle{

   friend class Building;

   double x, y; //position
   short age;
   double hV, vV; //velocity
   double offset;

   static SDL_Surface *screen_, *image_, *shadow_;

   const static double GRAVITY;
   const static int PARTICLE_COUNT;
   const static double VELOCITY_RANGE;
   const static int DELAY; //ms
   const static int DECAY;

public:
   Particle(const pixels_t xPos, const pixels_t yPos);

   void progress();

   void draw() const;

   bool expired() const;

   static void init(SDL_Surface *screen,
                    SDL_Surface *image,
                    SDL_Surface *shadow);
};

#endif