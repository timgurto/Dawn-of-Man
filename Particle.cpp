// (C) 2009 Tim Gurto

#include <sstream>
#include <cassert>
#include <list>
#include "Particle.h"

double Particle::GRAVITY = 1;
int Particle::PARTICLE_COUNT = 8;
double Particle::VELOCITY_RANGE = 4;
int Particle::DELAY = 25; //ms
int Particle::DECAY = 3;

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

void Particle::progress(){
   age -= DECAY;
   x += hV;
   offset += (hV >= 0 ? hV : -1 * hV);
   y -= vV;
   vV -= GRAVITY;
}

void Particle::draw() const{
   //SDL_SetAlpha(shadow_, SDL_SRCALPHA, Uint8(age));
   SDL_BlitSurface(shadow_, 0, screen_, &makeRect(int(x - offset),
                                                  int(y + offset)));
   //SDL_SetAlpha(image_, SDL_SRCALPHA, Uint8(age));
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