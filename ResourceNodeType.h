// (C) 2010 Tim Gurto

#ifndef RESOURCE_NODE_TYPE_H
#define RESOURCE_NODE_TYPE_H

#include "Resources.h"
#include "globals.h"

//TODO comment
class ResourceNodeType: public EntityType{
   friend class ResourceNode;

   //the inital, full amount of resources in the node
   Resources maxResources_;

   //how much is harvested at a time
   Resources yield_;

   //sprite color
   EntityColor color_;

public:
   ResourceNodeType(typeNum_t index,
                    const std::string &name,
                    const SDL_Rect &drawRect,
                    const SDL_Rect &baseRect,
                    const Point &selectionCenter,
                    const Resources &maxResources,
                    const Resources &yield,
                    EntityColor color);
};

#endif