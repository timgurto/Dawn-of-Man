// (C) 2009 Tim Gurto

#include "SDL.h"
#include "EntityType.h"
#include "misc.h"

EntityType::EntityType(typeNum_t index, EntityTypeID type,
                       const std::string &name,
                       const SDL_Rect &drawRect,
                       const SDL_Rect &baseRect,
                       const Point &selectionCenter,
                       const std::string &soundFile,
                       const std::string &deathSoundFile,
                       const std::string &hitSoundFile):
index_(index),
name_(name),
drawRect_(drawRect),
baseRect_(baseRect),
selectionCenter_(selectionCenter),
surface_(loadImage(makePath(type, index_), ENTITY_MASK)),
icon_(loadImage(makePath(type, index_, IMG_ICON), ENTITY_BACKGROUND)),
mask_(loadImage(makePath(type, index_, IMG_MASK), ENTITY_MASK)),
soundFile_(soundFile),
deathSoundFile_(deathSoundFile),
hitSoundFile_(hitSoundFile),
sound_(loadSound(SOUND_PATH + soundFile)),
deathSound_(loadSound(SOUND_PATH + deathSoundFile)),
hitSound_(loadSound(SOUND_PATH + hitSoundFile)){}

EntityType::EntityType(const EntityType &original):
index_(original.index_),
name_(original.name_),
drawRect_(original.drawRect_),
baseRect_(original.baseRect_),
selectionCenter_(original.selectionCenter_),
surface_(copySurface(original.surface_)),
icon_(copySurface(original.icon_)),
mask_(copySurface(original.mask_)),
soundFile_(original.soundFile_),
deathSoundFile_(original.deathSoundFile_),
hitSoundFile_(original.hitSoundFile_),
sound_(loadSound(SOUND_PATH + soundFile_)),
deathSound_(loadSound(SOUND_PATH + deathSoundFile_)),
hitSound_(loadSound(SOUND_PATH + hitSoundFile_)){}

EntityType::~EntityType(){
   freeSurface(surface_);
   freeSurface(icon_);
   freeSurface(mask_);
   freeSound(sound_);
   freeSound(deathSound_);
   freeSound(hitSound_);
}

const SDL_Rect &EntityType::getBaseRect() const{
   return baseRect_;
}

const SDL_Rect &EntityType::getDrawRect() const{
   return drawRect_;
}

SDL_Surface *EntityType::getIcon() const{
   return icon_;
}

const std::string &EntityType::getName() const{
   return name_;
}

typeNum_t EntityType::getIndex() const{
   return index_;
}

SDL_Sound *EntityType::getSound() const{
   return sound_;
}

SDL_Sound *EntityType::getHitSound() const{
   return hitSound_;
}