
#include "ComponentTimer.h"

#include <fstream>

ComponentTimer::ComponentTimer():
  m_range(1.0)
{
  sPatchOut po;

  po.dynamic = false;
  po.name = "Time";
  po.type = CT_DOUBLE;

  Component::InitializePatches(NULL, 0, &po, 1);

  m_timer.start();

  rwButton *btn = new rwButton(m_position, 10, rwFUNCTOR(OnButtonClicked), "",
    this);
}

ComponentTimer::~ComponentTimer()
{
}

void ComponentTimer::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();
  while (m_range > 0.0 && m_timer.getElapsedTimeInSec() > m_range)
    m_timer.modifyElapsedTime(-m_range * 1000 * 1000);
  SetOutput(0, m_timer.getElapsedTimeInSec());
}

void ComponentTimer::OnButtonClicked(ComponentTimer *caller, rwButtonEvent &e)
{
  if (e.type == rwBTN_CLICKED)
    caller->m_timer.start();
}

std::string ComponentTimer::GetNodeType()
{
  std::stringstream t;
  t << "Timer";
  return t.str();
}

void ComponentTimer::ToFile(std::ostream &out)
{
  out << "Timer";
  WriteDefaultParams(out);
}

Component *ComponentTimer::FromFile(std::istream &in)
{
  ComponentTimer *c = new ComponentTimer();
  c->ReadDefaultParams(in);

  return c;
}

Component *ComponentTimer::Create()
{
  return new ComponentTimer();
}

Component *ComponentTimer::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
