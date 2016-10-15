// (C) 2009 Tim Gurto

#include "SDL.h"
#include "EntityType.h"
#include "misc.h"

EntityType::EntityType(typeNum_t index, EntityTypeID type,
                       const std::string &name,
                       const SDL_Rect &drawRect,
                       const SDL_Rect &baseRect):
index_(index),
name_(name),
drawRect_(drawRect),
baseRect_(baseRect){
   surface = loadImage(makePath(type, index_),
                       ENTITY_MASK);
   icon = loadImage(makePath(type, index_, ICON),
                    ENTITY_BACKGROUND);
   mask = loadImage(makePath(type, index_, MASK),
                    ENTITY_MASK);
}

EntityType::EntityType(const EntityType &original):
index_(original.index_),
name_(original.name_),
drawRect_(original.drawRect_),
baseRect_(original.baseRect_),
surface(copySurface(original.surface)),
icon(copySurface(original.icon)),
mask(copySurface(original.mask)){}

EntityType &EntityType::operator=(const EntityType &rhs){
   if (&rhs != this){
      index_ = rhs.index_;
      name_ = rhs.name_;
      drawRect_ = rhs.drawRect_;
      baseRect_ = rhs.baseRect_;
      surface = copySurface(rhs.surface);
      icon = copySurface(rhs.icon);
      mask = copySurface(rhs.mask);
   }
   return *this;
}

EntityType::~EntityType(){
   freeSurface(surface);
   freeSurface(icon);
   freeSurface(mask);
}

SDL_Rect EntityType::getBaseRect() const{
   return baseRect_;
}

SDL_Rect EntityType::getDrawRect() const{
   return drawRect_;
}

SDL_Surface *EntityType::getIcon() const{
   return icon;
}