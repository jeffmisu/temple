
#include "ComponentDebugOutput.h"

#include <fstream>

ComponentDebugOutput::ComponentDebugOutput()
{
  sPatchIn pi[2];

  pi[0].comp = NULL;
  pi[0].dynamic = true;
  pi[0].name = "Value";
  pi[0].type = CT_NONE;

  pi[1].comp = NULL;
  pi[1].dynamic = false;
  pi[1].name = "Iterations";
  pi[1].type = CT_DOUBLE;

  Component::InitializePatches(pi, 2, NULL, 0);

  rwButton *btn = new rwButton(m_position, 10, rwFUNCTOR(OnButtonClicked), "", this);
}

ComponentDebugOutput::~ComponentDebugOutput()
{
}

void ComponentDebugOutput::Evaluate(int timestamp)
{
  if (m_inputs[0].comp == NULL)
    return;

  double its_d = 1;
  GetInput(1, &its_d);
  int its = its_d;

  if (its == 1)
  {
    m_inputs[0].comp->Evaluate(
      ADVANCE_TIMESTAMP(m_inputs[0].comp->GetTimestamp()));
    return;
  }

  std::ofstream out("dbg_out.txt", std::ios::out);
  for (int i = 0; i < its; i++)
  {
    m_inputs[0].comp->Evaluate(
      ADVANCE_TIMESTAMP(m_inputs[0].comp->GetTimestamp()));
    out << getValueString(m_inputs[0].value, m_inputs[0].type) << std::endl;
  }
  out.close();
}

void ComponentDebugOutput::OnButtonClicked(ComponentDebugOutput *caller,
  rwButtonEvent &e)
{
  if (e.type == rwBTN_CLICKED)
    caller->Evaluate(0);
}

std::string ComponentDebugOutput::GetNodeType()
{
  std::stringstream t;

  if (m_inputs[0].comp != NULL)
    t << "Output (" << getValueString(m_inputs[0].value, m_inputs[0].type) << ")";
  else
    t << "Output";

  return t.str();
}

void ComponentDebugOutput::ToFile(std::ostream &out)
{
  out << "Debug";
  WriteDefaultParams(out);
}

Component *ComponentDebugOutput::FromFile(std::istream &in)
{
  ComponentDebugOutput *c = new ComponentDebugOutput();
  c->ReadDefaultParams(in);

  return c;
}

Component *ComponentDebugOutput::Create()
{
  return new ComponentDebugOutput();
}

Component *ComponentDebugOutput::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
