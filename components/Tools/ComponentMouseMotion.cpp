
#include "ComponentMouseMotion.h"

#include <fstream>

ComponentMouseMotion::ComponentMouseMotion()
{
  sPatchOut po[2];

  po[0].type = CT_DOUBLE;
  po[0].name = "x motion";
  po[0].dynamic = false;

  po[1].type = CT_DOUBLE;
  po[1].name = "y motion";
  po[1].dynamic = false;

  Component::InitializePatches(NULL, 0, po, 2);
}

ComponentMouseMotion::~ComponentMouseMotion()
{
}

void ComponentMouseMotion::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();

  if (!(m_root->m_mBtnStates & rwMOUSE_LEFT))
  {
    SetOutput(0, 0.0);
    SetOutput(1, 0.0);
    return;
  }

  SetOutput(0, m_root->m_mPosition.x - m_root->m_mOrigin.x / 512.0);
  SetOutput(1, m_root->m_mPosition.y - m_root->m_mOrigin.y / 512.0);
}

std::string ComponentMouseMotion::GetNodeType()
{
  std::stringstream t;
  t << "Mouse motion";
  return t.str();
}

void ComponentMouseMotion::ToFile(std::ostream &out)
{
  out << "MouseMotion";
  WriteDefaultParams(out);
}

Component *ComponentMouseMotion::FromFile(std::istream &in)
{
  ComponentMouseMotion *c = new ComponentMouseMotion();
  c->ReadDefaultParams(in);

  return c;
}

Component *ComponentMouseMotion::Create()
{
  return new ComponentMouseMotion();
}

Component *ComponentMouseMotion::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
