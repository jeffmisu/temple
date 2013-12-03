
#ifndef COMPONENTIF_H
#define COMPONENTIF_H

#include "../../Component.h"

//CMP_NAME=If

class ComponentIf: public Component
{
  public:
  ComponentIf();
  ~ComponentIf();

  void Evaluate(int timestamp);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
