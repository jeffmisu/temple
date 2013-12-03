
#ifndef COMPONENTGRAPH_H
#define COMPONENTGRAPH_H

#include <string>
#include <iostream>

class Component;

struct cgNode
{
  // A static factory function
  Component *(*factory)();
  
  // Deserializing function
  Component *(*fromFile)(std::istream &);

  // The name of the component type
  std::string name;
  
  // The name of the class
  std::string cname;
};

extern cgNode cmpGraph[];
extern int cmpTotal;

#endif
