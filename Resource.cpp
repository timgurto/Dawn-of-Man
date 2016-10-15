#include "Resource.h"
#include "GameData.h"

Resource::Resource(typeNum_t type, const Point &loc):
Entity(type, loc){}

const EntityType &Resource::type() const{
   return game_->resourceTypes[typeIndex_];
}

EntityTypeID Resource::classID() const{
   return ENT_RESOURCE;
}