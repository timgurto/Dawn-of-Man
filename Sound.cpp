// (C) 2010 Tim Gurto

#include <cassert>
#include "SDL_mixer.h"
#include "Sound.h"
#include "globals.h"

int Sound::soundsLoaded_ = 0;

Sound::Sound(const std::string fileName):
fileName_(fileName),
sound_(loadSound(fileName)){}

Sound::Sound(const Sound &original):
fileName_(original.fileName_),
sound_(loadSound(fileName_)){}

Sound::~Sound(){
   unloadSound(sound_);
}

Sound &Sound::operator=(const Sound &rhs){
   if (this != &rhs){
      unloadSound(sound_);
      fileName_ = rhs.fileName_;
      sound_ = loadSound(fileName_);
   }
   return *this;
}

void Sound::play() const{
   if (!DEBUG)
      if (sound_)
         Mix_PlayChannel(-1, sound_, 0);
}

void Sound::quit(){
   assert(soundsLoaded_ == 0);
}

Mix_Chunk *Sound::loadSound(const std::string &fileName){
   Mix_Chunk *sound = 0;
   if (fileName != "" && fileName != SOUND_PATH){
      sound = Mix_LoadWAV(fileName.c_str());
      assert(sound);
      ++soundsLoaded_;
   }
   return sound;
}

void Sound::unloadSound(Mix_Chunk *&sound){
   if (sound){
      Mix_FreeChunk(sound);
      --soundsLoaded_;
      sound = 0;
   }
}