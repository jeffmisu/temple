
#ifndef COMPONENTINC_H
#define COMPONENTINC_H

#include "../../Component.h"

//CMP_NAME=Incrementer

class ComponentInc: public Component
{
  public:
  ComponentInc();
  ~ComponentInc();

  double m_signal;

  void SetValue(double value);

  void Evaluate(int timestamp);

  void OnRightClicked();
  static void OnTextEntered(ComponentInc *caller, rwTextBoxEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
