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
   icon_ = loadImage(makePath(type, index_, IMG_ICON),
                    ENTITY_BACKGROUND);
   mask_ = loadImage(makePath(type, index_, IMG_MASK),
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

EntityType::~EntityType(){
   freeSurface(surface_);
   freeSurface(icon_);
   freeSurface(mask_);
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