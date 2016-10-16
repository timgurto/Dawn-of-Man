// (C) 2009 Tim Gurto

#include "EntityType.h"
#include "misc.h"
#include "util.h"

EntityType::EntityType(typeNum_t index, EntityTypeID type,
                       const std::string &name,
                       const SDL_Rect &drawRect,
                       const SDL_Rect &baseRect,
                       const Point &selectionCenter,
                       typeNum_t decorationAtDeath,
                       const std::string &soundFile,
                       const std::string &deathSoundFile,
                       const std::string &hitSoundFile):
index_(index),
name_(name),
drawRect_(drawRect),
baseRect_(baseRect),
selectionCenter_(selectionCenter),
decorationAtDeath_(decorationAtDeath),
surface_(makePath(type, index_), ENTITY_MASK),
icon_(makePath(type, index_, IMG_ICON), ENTITY_BACKGROUND),
mask_(makePath(type, index_, IMG_MASK), ENTITY_MASK),
soundFile_(soundFile),
deathSoundFile_(deathSoundFile),
hitSoundFile_(hitSoundFile),
sound_(SOUND_PATH + soundFile),
deathSound_(SOUND_PATH + deathSoundFile),
hitSound_(SOUND_PATH + hitSoundFile){}
//TODO remove sound files
EntityType::EntityType(const EntityType &original):
index_(original.index_),
name_(original.name_),
drawRect_(original.drawRect_),
baseRect_(original.baseRect_),
selectionCenter_(original.selectionCenter_),
decorationAtDeath_(original.decorationAtDeath_),
surface_(original.surface_),
icon_(original.icon_),
mask_(original.mask_),
soundFile_(original.soundFile_),
deathSoundFile_(original.deathSoundFile_),
hitSoundFile_(original.hitSoundFile_),
sound_(SOUND_PATH + soundFile_),
deathSound_(SOUND_PATH + deathSoundFile_),
hitSound_(SOUND_PATH + hitSoundFile_){}

EntityType::~EntityType(){}

const SDL_Rect &EntityType::getBaseRect() const{
   return baseRect_;
}

const SDL_Rect &EntityType::getDrawRect() const{
   return drawRect_;
}

const Surface &EntityType::getIcon() const{
   return icon_;
}

const std::string &EntityType::getName() const{
   return name_;
}

typeNum_t EntityType::getIndex() const{
   return index_;
}

const Sound &EntityType::getSound() const{
   return sound_;
}

const Sound &EntityType::getHitSound() const{
   return hitSound_;
}

typeNum_t EntityType::getDecorationAtDeath() const{
   return decorationAtDeath_;
}