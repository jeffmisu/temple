
#ifndef COMPONENTDEBUGOUTPUT_H
#define COMPONENTDEBUGOUTPUT_H

#include "../../Component.h"

//CMP_NAME=Debug output

class ComponentDebugOutput: public Component
{
  public:
  ComponentDebugOutput();
  ~ComponentDebugOutput();

  void Evaluate(int timestamp);

  static void OnButtonClicked(ComponentDebugOutput *caller, rwButtonEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
