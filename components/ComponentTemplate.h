
// This file outlines the implementation of a basic Component. Use it as a
// template, but don't modify it directly, because the newcomp.py script also
// uses it. See the file 'Component.h' for more information about the purpose
// of each function / variable.

#ifndef COMPONENTTEMPLATE_H
#define COMPONENTTEMPLATE_H

#include "Component.h"

//CMP_NAME=Template

class ComponentTemplate: public Component
{
  public:
  ComponentTemplate();
  ~ComponentTemplate();

  void Evaluate(int timestamp);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif

