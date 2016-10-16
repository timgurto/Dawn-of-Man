// (C) 2010 Tim Gurto

#ifndef SOUND_H
#define SOUND_H

#include <string>
#include "SDL_mixer.h"

class Sound{
   //for reloading upon copy or assign
   std::string fileName_;
   
   //the actual sound pointer
   Mix_Chunk *sound_;

   //Keeps track of allocated sounds
   static int soundsLoaded_;

   //load a sound; called by c'tors and assignment
   static Mix_Chunk *loadSound(const std::string &fileName);

   //unload a sound; called by the d'tor and assignment
   static void unloadSound(Mix_Chunk *&sound);

public:
   Sound(const std::string fileName = "");
   Sound(const Sound &original);
   ~Sound();

   Sound &operator=(const Sound &rhs);

   //play the sound
   void play() const;

   static void quit();
};

#endif