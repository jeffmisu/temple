
#include "ComponentOp2.h"

#include <fstream>

const sPatchIn ComponentOp2::s_blueprintIn[] =
{
  sPatchIn(CT_DOUBLE, "Input A"),
  sPatchIn(CT_DOUBLE, "Input B")
};

ComponentOp2::ComponentOp2():
  m_operator('+')
{
  sPatchOut po;
  po.type = CT_DOUBLE;
  po.name = "Value";
  po.dynamic = false;
  po.value = NULL;
  InitializePatches(s_blueprintIn, sizeof(s_blueprintIn) / sizeof(sPatchIn),
    &po, 1);
}

ComponentOp2::~ComponentOp2()
{
}

ComponentOp2 &ComponentOp2::operator=(const ComponentOp2 &s)
{
  Component::operator=(s);

  m_operator = s.m_operator;

  return *this;
}

void ComponentOp2::SetOperator(char op)
{
  ComponentType inType = CT_NONE;
  if (op == '|' || op == '&')
    inType = CT_BOOL;
  else if (op == '=')
    inType = CT_NONE;
  else
    inType = CT_DOUBLE;

  if (inType == CT_NONE)
  {
    for (size_t i = 0; i < m_inputs.size(); i++)
    {
      if (m_inputs[i].comp == NULL)
        m_inputs[i].type = CT_NONE;
      else
        inType = m_inputs[i].type;
      m_inputs[i].dynamic = true;
    }
    for (size_t i = 0; i < m_inputs.size(); i++)
      if (m_inputs[i].dynamic)
        m_inputs[i].type = inType;
  }
  else
  {
    for (size_t i = 0; i < m_inputs.size(); i++)
      if (m_inputs[i].type != inType && inType != CT_NONE)
      {
        m_inputs[i].type = inType;
        m_inputs[i].dynamic = false;
        RemoveInput(m_inputs[i].comp);
      }
  }

  if (op == '|' || op == '&' || op == '<' || op == '>' || op == '=')
  {
    if (m_outputs[0].type != CT_BOOL)
      ClearOutputs();
    m_outputs[0].type = CT_BOOL;
  }
  else
  {
    if (m_outputs[0].type != CT_DOUBLE)
      ClearOutputs();
    m_outputs[0].type = CT_DOUBLE;
  }

  m_operator = op;
}

void ComponentOp2::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();

  if (m_inputs[0].comp == NULL || m_inputs[1].comp == NULL)
  {
    assignDefaultValue(m_outputs[0].value, m_outputs[0].type);
    return;
  }

  // There are certainly better ways to do this, but I'm not using them yet

  double v0d, v1d;
  bool v0b, v1b;
  void *v0, *v1;

  if      (m_inputs[0].type == CT_DOUBLE) v0 = &v0d;
  else if (m_inputs[0].type == CT_BOOL)   v0 = &v0b;
  if      (m_inputs[1].type == CT_DOUBLE) v1 = &v1d;
  else if (m_inputs[1].type == CT_BOOL)   v1 = &v1b;

  if (PASS_TIMESTAMP(m_inputs[0]))
    m_inputs[0].comp->Evaluate(timestamp);
  if (PASS_TIMESTAMP(m_inputs[1]))
    m_inputs[1].comp->Evaluate(timestamp);
  assignValue(v0, m_inputs[0].value, m_inputs[0].type);
  assignValue(v1, m_inputs[1].value, m_inputs[1].type);

  if (m_operator == '+')
    *(double *)m_outputs[0].value = *(double *)v0 + *(double *)v1;
  else if (m_operator == '-')
    *(double *)m_outputs[0].value = *(double *)v0 - *(double *)v1;
  else if (m_operator == '*')
    *(double *)m_outputs[0].value = *(double *)v0 * *(double *)v1;
  else if (m_operator == '/')
    *(double *)m_outputs[0].value = *(double *)v0 / *(double *)v1;

  else if (m_operator == '<')
    *(bool *)m_outputs[0].value = *(double *)v0 < *(double *)v1;
  else if (m_operator == '>')
    *(bool *)m_outputs[0].value = *(double *)v0 > *(double *)v1;

  else if (m_operator == '|')
    *(bool *)m_outputs[0].value = *(bool *)v0 || *(bool *)v1;
  else if (m_operator == '&')
    *(bool *)m_outputs[0].value = *(bool *)v0 && *(bool *)v1;

  else if (m_operator == '=')
  {
    if (m_inputs[0].type == CT_DOUBLE)
      *(bool *)m_outputs[0].value = *(double *)v0 == *(double *)v1;
    else if (m_inputs[0].type == CT_BOOL)
      *(bool *)m_outputs[0].value = *(bool *)v0 == *(bool *)v1;
  }
}

void ComponentOp2::OnRightClicked()
{
  rwContextMenu *cm = new rwContextMenu(Component::m_position, rwFUNCTOR(OnSelection), this);
  cm->AddChoice("+: Addition", (void *)'+');
  cm->AddChoice("*: Multiplication", (void *)'*');
  cm->AddChoice("-: Subtraction", (void *)'-');
  cm->AddChoice("/: Division", (void *)'/');
  cm->AddChoice("=: Comparison", (void *)'=');
  cm->AddChoice("<: Less than", (void *)'<');
  cm->AddChoice(">: Greater than", (void *)'>');
  cm->AddChoice("|: Logical or", (void *)'|');
  cm->AddChoice("&: Logical and", (void *)'&');
}

void ComponentOp2::OnSelection(ComponentOp2 *caller, rwContextMenuEvent &e)
{
  if (e.type == rwCONTEXT_SELECT)
    caller->SetOperator((int)e.data);
}

std::string ComponentOp2::GetNodeType()
{
  std::stringstream t;
  t << "Operator (" << m_operator << ")";

  return t.str();
}

void ComponentOp2::ToFile(std::ostream &out)
{
  out << "Op2";
  WriteDefaultParams(out);
  out << m_operator;
}

Component *ComponentOp2::FromFile(std::istream &in)
{
  ComponentOp2 *c = new ComponentOp2();
  c->ReadDefaultParams(in);
  char op;
  in >> op;
  c->SetOperator(op);

  return c;
}

Component *ComponentOp2::Create()
{
  return new ComponentOp2();
}

Component *ComponentOp2::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
