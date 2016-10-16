// (C) 2009-2010 Tim Gurto
#ifndef PARTICLE_H
#define PARTICLE_H

#include <cstdlib>
#include "types.h"
#include "misc.h"

struct GameData;
class Surface;

//Describes a particle.  At this point the images and
//trajectories used are static, and so each object
//keeps track only of its location, age and velocities.
class Particle{

   double x_, y_; //position
   double age_; //how long the particle has existed
   double hV_, vV_; //velocity
   double offset_; //lateral displacement, for shadow

   //static pointers to surfaces
   static Surface
      *image_, //the particle image
      *shadow_; //the particle shadow image

public:
   
   //whether particles fade over time
   static const bool PARTICLE_FADE;

   //How much particles' verticle velocity increases
   //over time
   static const double GRAVITY;

   //How many particles should exist at once
   static const int PARTICLE_COUNT;

   //The range of starting velocities.
   //Higher = wider and more varying trajectories
   //in pixels/tick
   static const double VELOCITY_RANGE;

   //How much particles age per tick.
   static const int DECAY;

   Particle(const pixels_t x, const pixels_t y);

   //updates to velocity and position
   void tick(double delta);

   //renders the particle and shadow
   void draw(const GameData &game) const;

   //whether the particle has aged enough that it
   //should be removed
   bool expired() const;

   //initializes static pointers
   static void init(Surface *image,
                    Surface *shadow);
};

#endif