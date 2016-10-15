// (C) 2009 Tim Gurto

#include "SDL.h"
#include "EntityType.h"
#include "misc.h"

EntityType::EntityType(typeNum_t index, EntityTypeID type,
                       const std::string &name,
                       const SDL_Rect &drawRect,
                       const SDL_Rect &baseRect,
                       const Point &selectionCenter):
index_(index),
name_(name),
drawRect_(drawRect),
baseRect_(baseRect),
selectionCenter_(selectionCenter){
   surface_ = loadImage(makePath(type, index_),
                       ENTITY_MASK);
   icon_ = loadImage(makePath(type, index_, ICON),
                    ENTITY_BACKGROUND);
   mask_ = loadImage(makePath(type, index_, MASK),
                    ENTITY_MASK);
}

EntityType::EntityType(const EntityType &original):
index_(original.index_),
name_(original.name_),
drawRect_(original.drawRect_),
baseRect_(original.baseRect_),
selectionCenter_(original.selectionCenter_),
surface_(copySurface(original.surface_)),
icon_(copySurface(original.icon_)),
mask_(copySurface(original.mask_)){}

EntityType &EntityType::operator=(const EntityType &rhs){
   if (&rhs != this){
      index_ = rhs.index_;
      name_ = rhs.name_;
      drawRect_ = rhs.drawRect_;
      baseRect_ = rhs.baseRect_;
      selectionCenter_ = rhs.selectionCenter_;
      surface_ = copySurface(rhs.surface_);
      icon_ = copySurface(rhs.icon_);
      mask_ = copySurface(rhs.mask_);
   }
   return *this;
}

EntityType::~EntityType(){
   freeSurface(surface_);
   freeSurface(icon_);
   freeSurface(mask_);
}

SDL_Rect EntityType::getBaseRect() const{
   return baseRect_;
}

SDL_Rect EntityType::getDrawRect() const{
   return drawRect_;
}

SDL_Surface *EntityType::getIcon() const{
   return icon_;
}

const std::string &EntityType::getName(){
   return name_;
}