// (C) 2009-2010 Tim Gurto
//TODO comments
#ifndef PARTICLE_H
#define PARTICLE_H

#include <cstdlib>
#include "SDL.h"
#include "types.h"
#include "misc.h"

struct GameData;

class Particle{

   double x_, y_; //position
   double age_; //how long the particle has existed
   double hV_, vV_; //velocity
   double offset_; //lateral displacement, for shadow

   //static pointers to surfaces
   static SDL_Surface
      *screen_,
      *image_, //the particle image
      *shadow_; //the particle shadow image

public:

   //constants
   const static double GRAVITY;
   const static int PARTICLE_COUNT;
   const static double VELOCITY_RANGE; //in pixels/tick
   const static int DELAY; //in ms
   const static int DECAY;

   Particle(const pixels_t x, const pixels_t y);

   //updates to velocity and position
   void tick(double delta);

   void draw(const GameData &game) const;

   bool expired() const;

   static void init(SDL_Surface *screen,
                    SDL_Surface *image,
                    SDL_Surface *shadow);
};

#endif