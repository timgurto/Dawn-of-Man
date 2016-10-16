// (C) 2010 Tim Gurto

#include <vector>
#include <sstream>
#include <string>
#include <numeric>
#include "Resources.h"
#include "globals.h"

size_t Resources::resourceCount_ = 0;
std::vector<std::string> Resources::names_;   

Resources::Resources():
vals_(resources_t(resourceCount_, 0)){}

Resources::Resources(const resources_t &v):
vals_(v){}

void Resources::init(std::vector <std::string> &names){
   resourceCount_ = names.size();
   names_ = names;
}

Resources Resources::empty(){
   return resources_t(resourceCount_, 0);
}

Resources &Resources::operator+=(const Resources &rhs){
   for (size_t i = 0; i != resourceCount_; ++i)
      vals_[i] += rhs.vals_[i];
   return *this;
}

Resources Resources::operator+(const Resources &rhs) const{
   Resources result = *this;
   result += rhs;
   return result;
}

Resources &Resources::operator-=(const Resources &rhs){
   for (size_t i = 0; i != resourceCount_; ++i)
      if (vals_[i] > rhs.vals_[i])
         vals_[i] -= rhs.vals_[i];
      else
         vals_[i] = 0;
   return *this;
}

Resources Resources::operator-(const Resources &rhs) const{
   Resources result = *this;
   result -= rhs;
   return result;
}

bool Resources::operator>=(const Resources &rhs) const{
   for (size_t i = 0; i != resourceCount_; ++i)
      //check all resources are >= rhs
      if (vals_[i] < rhs.vals_[i])
         return false;
   return true;
}

bool Resources::operator<(const Resources &rhs) const{
   return !(*this >= rhs);
}

std::ostream &operator<<(std::ostream &os,
                         const Resources &res){
   bool blankDone = false;
   for (size_t i = 0; i != Resources::resourceCount_; ++i){
      if (res.vals_[i] > 0){
         if (!blankDone)
            blankDone = true;
         else
            os << ", ";
         os << res.vals_[i] << " " << Resources::names_[i];
      }
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

resource_t Resources::sum() const{
   return std::accumulate(vals_.begin(),
                          vals_.end(),
                          0);
}

size_t Resources::getResourceCount(){
   return resourceCount_;
}

typeNum_t Resources::findResource(std::string name){
   for (size_t i = 0; i != resourceCount_; ++i){
      if (names_[i] == name)
         return i;
   }
   return NO_TYPE;
}