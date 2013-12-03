
#ifndef COMPONENTMOUSEMOTION_H
#define COMPONENTMOUSEMOTION_H

#include "../../Component.h"

//CMP_NAME=Mouse motion

class ComponentMouseMotion: public Component
{
  public:
  ComponentMouseMotion();
  ~ComponentMouseMotion();

  void Evaluate(int timestamp);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
