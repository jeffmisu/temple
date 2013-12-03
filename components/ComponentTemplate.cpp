
// This file outlines the implementation of a basic Component. Use it as a
// template, but don't modify it directly, because the newcomp.py script also
// uses it. See the file 'Component.h' for more information about the purpose
// of each function / variable.

#include "ComponentTemplate.h"

const sPatchIn blueprintInp[] = {
  sPatchIn(CT_DOUBLE, "Input")
};

const sPatchOut blueprintOut[] = {
  sPatchOut(CT_DOUBLE, "Output")
};

ComponentTemplate::ComponentTemplate()
{
  InitializePatches(
    blueprintIn,  sizeof(blueprintIn)  / sizeof(sPatchIn),
    blueprintOut, sizeof(blueprintOut) / sizeof(sPatchOut)
  );
}

ComponentTemplate::~ComponentTemplate()
{
}

void ComponentTemplate::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();
}

std::string ComponentTemplate::GetNodeType()
{
  std::stringstream t;
  t << "Template";

  return t.str();
}

void ComponentTemplate::ToFile(std::ostream &out)
{
  out << "Template";
  WriteDefaultParams(out);
}

Component *ComponentTemplate::FromFile(std::istream &in)
{
  ComponentTemplate *c = new ComponentTemplate();
  c->ReadDefaultParams(in);

  return c;
}

Component *ComponentTemplate::Create()
{
  return new ComponentTemplate();
}

Component *ComponentTemplate::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}

