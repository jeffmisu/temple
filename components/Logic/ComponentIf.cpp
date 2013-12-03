
#include "ComponentIf.h"

#include <fstream>

const sPatchIn blueprint[] =
{
  sPatchIn(CT_BOOL, "Condition"),
  sPatchIn(CT_NONE, "True branch", true),
  sPatchIn(CT_NONE, "False branch", true)
};

ComponentIf::ComponentIf()
{
  sPatchOut po;
  po.type = CT_NONE;
  po.name = "Value";
  po.dynamic = true;
  po.value = NULL;
  InitializePatches(blueprint, sizeof(blueprint) / sizeof(sPatchIn), &po, 1);
}

ComponentIf::~ComponentIf()
{
}

void ComponentIf::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();

  if (m_inputs[1].comp == NULL || m_inputs[2].comp == NULL)
  {
    assignDefaultValue(m_outputs[0].value, m_outputs[0].type);
    return;
  }

  bool cond = true;
  GetInput(0, &cond);

  if (cond)
  {
    if (PASS_TIMESTAMP(m_inputs[1]))
      m_inputs[1].comp->Evaluate(timestamp);
    assignValue(m_outputs[0].value, m_inputs[1].value, m_outputs[0].type);
  }
  else
  {
    if (PASS_TIMESTAMP(m_inputs[2]))
      m_inputs[2].comp->Evaluate(timestamp);
    assignValue(m_outputs[0].value, m_inputs[2].value, m_outputs[0].type);
  }
}

std::string ComponentIf::GetNodeType()
{
  return "'if' block";
}

void ComponentIf::ToFile(std::ostream &out)
{
  out << "If";
  WriteDefaultParams(out);
}

Component *ComponentIf::FromFile(std::istream &in)
{
  ComponentIf *c = new ComponentIf();
  c->ReadDefaultParams(in);

  return c;
}

Component *ComponentIf::Create()
{
  return new ComponentIf();
}

Component *ComponentIf::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
