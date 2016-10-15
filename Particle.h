// (C) 2009 Tim Gurto
//TODO comments
#ifndef PARTICLE_H
#define PARTICLE_H

#include <cstdlib>
#include "SDL.h"
#include "globals.h"
#include "misc.h"
//TODO Maybe just have one container of all particles
class Particle{

   friend class Building;

   double x, y; //position
   short age;
   double hV, vV; //velocity
   double offset;

   static SDL_Surface *screen_, *image_, *shadow_;

   //TODO make const
   static double GRAVITY;
   static int PARTICLE_COUNT;
   static double VELOCITY_RANGE;
   static int DELAY; //ms
   static int DECAY;

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