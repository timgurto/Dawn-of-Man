// (C) 2009-2010 Tim Gurto

#include <sstream>
#include <cassert>
#include <list>
#include "Particle.h"
#include "GameData.h"

const bool Particle::PARTICLE_FADE = false;
const double Particle::GRAVITY = 1.6;
const int Particle::PARTICLE_COUNT = 5;
const double Particle::VELOCITY_RANGE = 2;
const int Particle::DECAY = 6;

SDL_Surface
   *Particle::screen_ = 0,
   *Particle::image_ = 0,
   *Particle::shadow_ = 0;

Particle::Particle(const pixels_t x, const pixels_t y):
x_(x),
y_(y),
age_(255),
offset_(0){
   hV_ = rand() * 2 * VELOCITY_RANGE / RAND_MAX - VELOCITY_RANGE;
   vV_ = rand() * 2 * VELOCITY_RANGE / RAND_MAX - VELOCITY_RANGE;
}

//TODO ParticleType class with tick() function pointer (if multiple particle types are used)
void Particle::tick(double delta){
   age_ -= delta * DECAY;
   x_ += delta * hV_;
   offset_ += (hV_ >= 0 ? delta * hV_ : -1 * delta * hV_);
   y_ -= delta * vV_;
   vV_ -= delta * GRAVITY;
}

void Particle::draw(const GameData &game) const{
   SDL_Rect dest = makeRect(Sint16(x_), Sint16(y_)) + game.map;

   //shadow
   SDL_Rect disp = makeRect(int(dest.x - offset_), int(dest.y + offset_));
   SDL_BlitSurface(shadow_, 0, screen_, &disp);

   //particle
   if (PARTICLE_FADE)
      SDL_SetAlpha(image_, SDL_SRCALPHA, Uint8(age_));
   SDL_BlitSurface(image_, 0, screen_, &dest);
}

bool Particle::expired() const{
   return (age_ <= 0);
}

void Particle::init(SDL_Surface *screen, SDL_Surface *image,
                    SDL_Surface *shadow){
   image_ = image;
   screen_ = screen;
   shadow_ = shadow;
}