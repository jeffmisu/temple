
#ifndef COMPONENTRENDERER_H
#define COMPONENTRENDERER_H

#include "../../Component.h"

//CMP_NAME=Renderer

class ComponentRenderer: public Component
{
  protected:
  double *m_records;
  int m_recordCount;

  public:
  ComponentRenderer();
  ~ComponentRenderer();

  void SetRecordCount(int ct);
  void Render();

  void Evaluate(int timestamp);

  void OnRightClicked();
  static void OnTextEntered(ComponentRenderer *caller, rwTextBoxEvent &e);
  static void OnButtonClicked(ComponentRenderer *caller, rwButtonEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
