// (C) 2009-2010 Tim Gurto

#include <sstream>
#include <cassert>
#include <list>
#include "Particle.h"
#include "GameData.h"

const double Particle::GRAVITY = 1.6;
const int Particle::PARTICLE_COUNT = 15;
const double Particle::VELOCITY_RANGE = 1.5;
const int Particle::DELAY = 25; //ms
const int Particle::DECAY = 6;

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
   vV -= delta * GRAVITY;
}

void Particle::draw(const GameData &game) const{
   SDL_Rect dest = makeRect(int(x), int(y)) + game.map;

   //shadow
   SDL_Rect disp = makeRect(int(dest.x - offset), int(dest.y + offset));
   SDL_BlitSurface(shadow_, 0, screen_, &disp);

   //particle
   if (PARTICLE_FADE)
      SDL_SetAlpha(image_, SDL_SRCALPHA, Uint8(age));
   SDL_BlitSurface(image_, 0, screen_, &dest);
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