// (C) 2010 Tim Gurto

#ifndef RESOURCES_H
#define RESOURCES_H

#include <vector>
#include <iostream>
#include "types.h"

typedef std::vector<resource_t> resources_t;

class Resources{

   friend class ResourceNode;

   friend std::ostream &operator<<(std::ostream &os,
                                   const Resources &res);

   //the number and names of the different
   //resource types
   static size_t resourceCount_;
   static std::vector<std::string> names_;

   std::vector<resource_t> vals_;

public:
   Resources();
   Resources(const resources_t &v);

   Resources &operator+=(const Resources &rhs);
   Resources operator+(const Resources &rhs) const;

   //minus: any deficit in rhs
   Resources &operator-=(const Resources &rhs);
   Resources operator-(const Resources &rhs) const;

   //multiplication: scalar
   Resources &operator*=(double rhs);
   Resources operator*(double rhs) const;

   //whether each resource is higher in the lhs
   bool operator>=(const Resources &rhs) const;
   bool operator<(const Resources &rhs) const;

   bool isEmpty() const;

   static void init(std::vector <std::string> &names);

   static typeNum_t findResource(std::string name);

   //equivalent to no resources
   static Resources empty();

   std::string str() const;

   resource_t sum() const;

   static size_t getResourceCount();
};

#endif