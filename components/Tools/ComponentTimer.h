
#ifndef COMPONENTTIMER_H
#define COMPONENTTIMER_H

#include "../../Component.h"
#include "Timer.h"

//CMP_NAME=Timer

class ComponentTimer: public Component
{
  protected:
  Timer m_timer;
  double m_range;

  public:
  ComponentTimer();
  ~ComponentTimer();

  void Evaluate(int timestamp);

  static void OnButtonClicked(ComponentTimer *caller, rwButtonEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
