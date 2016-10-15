// (C) 2009 Tim Gurto

#include "EntityPtr.h"

EntityPtr::EntityPtr(Entity *ptr):
ptr_(ptr){}

bool EntityPtr::operator<(const EntityPtr &rhs) const{
   return ptr_->loc_.x < rhs.ptr_->loc_.x;
}

Entity *EntityPtr::operator->(){
   return ptr_;
}