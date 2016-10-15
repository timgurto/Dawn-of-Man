// (C) 2009 Tim Gurto

#include "EntityType.h"
#include "misc.h"

EntityType::EntityType(typeNum_t index, EntityTypeID type,
           const std::string &name,
           const SDL_Rect &drawRect, const SDL_Rect &baseRect,
           const SDL_Color &background):
index_(index),
name_(name),
drawRect_(drawRect),
baseRect_(baseRect){
   surface = loadImage(makePath(type, index_), background);
   portrait = loadImage(makePath(type, index_, true), background);
}

EntityType::EntityType(const EntityType &original):
index_(original.index_),
name_(original.name_),
drawRect_(original.drawRect_),
baseRect_(original.baseRect_),
surface(copySurface(original.surface)),
portrait(copySurface(original.portrait)){}

EntityType &EntityType::operator=(const EntityType &rhs){
   if (&rhs != this){
      index_ = rhs.index_;
      name_ = rhs.name_;
      drawRect_ = rhs.drawRect_;
      baseRect_ = rhs.baseRect_;
      surface = copySurface(rhs.surface);
      portrait = copySurface(rhs.portrait);
   }
   return *this;
}

EntityType::~EntityType(){
   freeSurface(surface);
   freeSurface(portrait);
}