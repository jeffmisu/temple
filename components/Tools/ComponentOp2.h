
#ifndef COMPONENTOP2_H
#define COMPONENTOP2_H

#include "../../Component.h"

//CMP_NAME=Operator

class ComponentOp2: public Component
{
  private:
  static const sPatchIn s_blueprintIn[];

  protected:
  char m_operator;

  public:
  ComponentOp2();
  ~ComponentOp2();

  ComponentOp2 &operator=(const ComponentOp2 &s);

  void SetOperator(char op);

  void Evaluate(int timestamp);

  void OnRightClicked();
  static void OnSelection(ComponentOp2 *caller, rwContextMenuEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
