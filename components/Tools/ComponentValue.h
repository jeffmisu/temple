
#ifndef COMPONENTCONST_H
#define COMPONENTCONST_H

#include "../../Component.h"

//CMP_NAME=Value

class ComponentValue: public Component
{
  public:
  ComponentValue();
  ~ComponentValue();

  ComponentValue &operator=(const ComponentValue &s);

  void SetValue(double value);

  void Evaluate(int timestamp);

  void OnRightClicked();
  static void OnTextEntered(ComponentValue *caller, rwTextBoxEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
