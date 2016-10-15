// (C) 2010 Tim Gurto

#include <vector>
#include <sstream>
#include <string>
#include "Resources.h"

size_t Resources::resourceCount_ = 0;
std::vector<std::string> Resources::names_;   

Resources::Resources():
vals_(resources_t(resourceCount_, 100)){}

Resources::Resources(const resources_t &v):
vals_(v){}

void Resources::init(size_t resourceCount,
                     std::vector <std::string> &names){
   resourceCount_ = resourceCount;
   for (size_t i = 0; i != resourceCount_; ++i){
      names_.push_back(names[i]);
   }
}

Resources Resources::empty(){
   return resources_t(resourceCount_, 0);
}

Resources &Resources::operator+=(const Resources rhs){
   for (size_t i = 0; i != resourceCount_; ++i)
      vals_[i] += rhs.vals_[i];
   return *this;
}

Resources &Resources::operator-=(const Resources rhs){
   for (size_t i = 0; i != resourceCount_; ++i)
      vals_[i] -= rhs.vals_[i];
   return *this;
}


bool Resources::operator>=(const Resources rhs) const{
   for (size_t i = 0; i != resourceCount_; ++i)
      //check all resources are >= rhs
      if (vals_[i] < rhs.vals_[i])
         return false;
   return true;
}

std::ostream &operator<<(std::ostream &os,
                         const Resources &res){
   for (size_t i = 0; i != Resources::resourceCount_; ++i){
      os << res.vals_[i] << " " << Resources::names_[i];
      if (i != Resources::resourceCount_ - 1)
         os << ", ";
   }
   return os;
}

bool Resources::isEmpty() const{
   return (vals_ == empty().vals_);
}

std::string Resources::str() const{
   std::ostringstream os;
   os << *this;
   return os.str();
}