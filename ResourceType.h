// (C) 2010 Tim Gurto

#ifndef RESOURCE_TYPE_H
#define RESOURCE_TYPE_H

//TODO comment
class ResourceType: public EntityType{
   friend class Resource;

public:
   ResourceType(typeNum_t index,
                const std::string &name,
                const SDL_Rect &drawRect,
                const SDL_Rect &baseRect,
                const Point &selectionCenter);
};



#endif