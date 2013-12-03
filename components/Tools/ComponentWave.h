
#ifndef COMPONENTWAVE_H
#define COMPONENTWAVE_H

#include "../../Component.h"

//CMP_NAME=Wave

enum
{
  CW_SINE,
  CW_SQUARE,
  CW_SAWTOOTH
};

class ComponentWave: public Component
{
  protected:
  double m_phase;
  int m_waveType;

  public:
  ComponentWave();
  ~ComponentWave();

  void SetWaveType(int type);

  void Evaluate(int timestamp);

  void OnRightClicked();
  static void OnSelection(ComponentWave *caller, rwContextMenuEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
