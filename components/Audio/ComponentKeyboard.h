
#ifndef COMPONENTKEYBOARD_H
#define COMPONENTKEYBOARD_H

#include "../../Component.h"

//CMP_NAME=Keyboard

class ComponentKeyboard: public Component
{
  protected:
  double m_octaveScale;

  public:
  ComponentKeyboard();
  ~ComponentKeyboard();

  bool ProcessKeyEvent(rwKeyEvent &e);

  void Evaluate(int timestamp);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif

