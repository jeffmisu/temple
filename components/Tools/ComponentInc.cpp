
#include "ComponentInc.h"

#include <fstream>

const sPatchIn blueprint[] = {
  sPatchIn(CT_DOUBLE, "Delta"),
  sPatchIn(CT_DOUBLE, "Reset")
};

ComponentInc::ComponentInc()
{
  InitializePatches(blueprint, sizeof(blueprint) / sizeof(sPatchIn),
    CT_DOUBLE);

  m_signal = 0.0;
}

ComponentInc::~ComponentInc()
{
}

void ComponentInc::SetValue(double value)
{
  m_signal = value;
  SetOutput(0, m_signal);
}

void ComponentInc::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();

  double inc = 1.0, modulo = 0.0;
  GetInput(0, &inc);
  GetInput(1, &modulo);

  m_signal += inc;
  if (m_signal >= modulo && modulo >= 0.0)
    m_signal -= modulo;

  SetOutput(0, m_signal);
}

void ComponentInc::OnRightClicked()
{
  rwTextBox *t = new rwTextBox(Component::m_position, rwFUNCTOR(OnTextEntered), this);
  t->SetNumericFilter();
  t->SetText("");
}

void ComponentInc::OnTextEntered(ComponentInc *caller, rwTextBoxEvent &e)
{
  if (e.type == rwTEXT_RETURN)
    sscanf(e.text.c_str(), "%lf", (double *)caller->m_outputs[0].value);
}

std::string ComponentInc::GetNodeType()
{
  std::stringstream t;
  t << "Inc (" << getValueString(this->m_outputs[0].value, this->m_outputs[0].type) << ")";

  return t.str();
}

void ComponentInc::ToFile(std::ostream &out)
{
  out << "Inc";
  WriteDefaultParams(out);
  out << *(double *)m_outputs[0].value;
}

Component *ComponentInc::FromFile(std::istream &in)
{
  ComponentInc *c = new ComponentInc();
  c->ReadDefaultParams(in);
  in >> *(double *)c->m_outputs[0].value;

  return c;
}

Component *ComponentInc::Create()
{
  return new ComponentInc();
}

Component *ComponentInc::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
