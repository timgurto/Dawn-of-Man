// (C) 2009-2010 Tim Gurto

#include <sstream>
#include <cassert>
#include <list>
#include "Particle.h"

const double Particle::GRAVITY = 1.4;
const int Particle::PARTICLE_COUNT = 30;
const double Particle::VELOCITY_RANGE = 1.5;
const int Particle::DELAY = 25; //ms
const int Particle::DECAY = 4;

SDL_Surface
   *Particle::screen_ = 0,
   *Particle::image_ = 0,
   *Particle::shadow_ = 0;

Particle::Particle(const pixels_t xPos, const pixels_t yPos):
x(xPos),
y(yPos),
age(255),
offset(0){
   hV = rand() * 2 * VELOCITY_RANGE / RAND_MAX - VELOCITY_RANGE;
   vV = rand() * 2 * VELOCITY_RANGE / RAND_MAX - VELOCITY_RANGE;
}

void Particle::tick(double delta){
   age -= delta * DECAY;
   x += hV;
   offset += (hV >= 0 ? delta * hV : -1 * delta * hV);
   y -= delta * vV;
   if (y > SCREEN_HEIGHT)
      age = 0;
   vV -= delta * GRAVITY;
}

void Particle::draw() const{
   if (SHADOWS)
      SDL_BlitSurface(shadow_, 0, screen_, &makeRect(int(x - offset),
                                                     int(y + offset)));
   if (PARTICLE_FADE)
      SDL_SetAlpha(image_, SDL_SRCALPHA, Uint8(age));
   SDL_BlitSurface(image_, 0, screen_, &makeRect(int(x), int(y)));
}

bool Particle::expired() const{
   return (age <= 0);
}

void Particle::init(SDL_Surface *screen, SDL_Surface *image,
                    SDL_Surface *shadow){
   image_ = image;
   screen_ = screen;
   shadow_ = shadow;
}