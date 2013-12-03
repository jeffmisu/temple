
#ifndef COMPONENTWAV_H
#define COMPONENTWAV_H

#include "../../Component.h"
#include "WAV.h"

//CMP_NAME=WAV

class ComponentWAV: public Component
{
  protected:
  int m_index;
  WAV m_wav;
  std::string m_filename;

  public:
  ComponentWAV();
  ~ComponentWAV();

  void SetFile(std::string filename);

  void Evaluate(int timestamp);

  void OnRightClicked();
  static void OnTextEntered(ComponentWAV *caller, rwTextBoxEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
