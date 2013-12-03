
#ifndef COMPONENTAVERAGER_H
#define COMPONENTAVERAGER_H

#include "../../Component.h"

//CMP_NAME=Averager

class ComponentAverager: public Component
{
  protected:
  double *m_window;
  int m_windowSize;
  int m_index;

  // To avoid FP error, we use a second running average and switch between
  // them every so often.
  int m_operations;
  double m_remediator;

  public:
  ComponentAverager();
  ~ComponentAverager();

  void SetWindow(int sz);

  void Evaluate(int timestamp);

  void OnRightClicked();
  static void OnTextEntered(ComponentAverager *caller, rwTextBoxEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
