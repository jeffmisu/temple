
#include "ComponentWAV.h"

#include <sstream>
#include <fstream>

ComponentWAV::ComponentWAV():
  m_index(0)
{
  InitializePatches(NULL, 0, CT_DOUBLE);
}

ComponentWAV::~ComponentWAV()
{
}

void ComponentWAV::SetFile(std::string filename)
{
  m_filename = filename;
  m_wav.LoadFromFile(m_filename);
}

void ComponentWAV::OnRightClicked()
{
  rwTextBox *t = new rwTextBox(Component::m_position, rwFUNCTOR(OnTextEntered), this);
  t->SetAlphanumericFilter();
  t->SetText("");
}

void ComponentWAV::OnTextEntered(ComponentWAV *caller, rwTextBoxEvent &e)
{
  if (e.type == rwTEXT_RETURN)
    caller->SetFile(e.text);
}

void ComponentWAV::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();
  float v = m_wav.GetSample(m_index);
  m_index = (m_index + 1) % m_wav.GetSampleCount();
  SetOutput(0, v);
}

std::string ComponentWAV::GetNodeType()
{
  std::stringstream t;
  if (m_filename.size() > 20)
    t << "WAV (" << m_filename.substr(0, 20) << "..." << ")";
  else
    t << "WAV (" << m_filename << ")";

  return t.str();
}

void ComponentWAV::ToFile(std::ostream &out)
{
  out << "WAV";
  WriteDefaultParams(out);
  out << m_filename;
}

Component *ComponentWAV::FromFile(std::istream &in)
{
  ComponentWAV *c = new ComponentWAV();
  c->ReadDefaultParams(in);
  std::string fname;
  in.ignore(1, ' ');
  getline(in, fname);
  c->SetFile(fname);

  return c;
}

Component *ComponentWAV::Create()
{
  return new ComponentWAV();
}

Component *ComponentWAV::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
