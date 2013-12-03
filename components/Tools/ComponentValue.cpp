
#include "ComponentValue.h"

#include <fstream>

ComponentValue::ComponentValue()
{
  Component::InitializePatches(NULL, 0, CT_DOUBLE);
}

ComponentValue::~ComponentValue()
{
}

ComponentValue &ComponentValue::operator=(const ComponentValue &s)
{
  Component::operator=(s);

  return *this;
}

void ComponentValue::SetValue(double value)
{
  assignValue(m_outputs[0].value, &value, m_outputs[0].type);
}

void ComponentValue::Evaluate(int timestamp)
{
}

void ComponentValue::OnRightClicked()
{
  rwTextBox *t = new rwTextBox(Component::m_position,
    rwFUNCTOR(OnTextEntered), this);
  t->SetNumericFilter();
  t->SetText("");
}

void ComponentValue::OnTextEntered(ComponentValue *caller, rwTextBoxEvent &e)
{
  if (e.type == rwTEXT_RETURN)
    sscanf(e.text.c_str(), "%lf", (double *)caller->m_outputs[0].value);
}

std::string ComponentValue::GetNodeType()
{
  std::stringstream t;
  t << "Value (";
  t << getValueString(this->m_outputs[0].value, this->m_outputs[0].type);
  t << ")";

  return t.str();
}

void ComponentValue::ToFile(std::ostream &out)
{
  out << "Value";
  WriteDefaultParams(out);
  out << *(double *)m_outputs[0].value;
}

Component *ComponentValue::FromFile(std::istream &in)
{
  ComponentValue *c = new ComponentValue();
  c->ReadDefaultParams(in);
  in >> *(double *)c->m_outputs[0].value;

  return c;
}

Component *ComponentValue::Create()
{
  return new ComponentValue();
}

Component *ComponentValue::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
