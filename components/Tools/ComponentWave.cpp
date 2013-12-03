
#include "ComponentWave.h"

#include <fstream>

const sPatchIn blueprint[] =
{
  sPatchIn(CT_DOUBLE, "Frequency"),
  sPatchIn(CT_DOUBLE, "Amplitude"),
  sPatchIn(CT_DOUBLE, "Phase offset")
};

ComponentWave::ComponentWave():
  m_phase(0.0),
  m_waveType(CW_SINE)
{
  InitializePatches(blueprint, sizeof(blueprint) / sizeof(sPatchIn),
    CT_DOUBLE);
}

ComponentWave::~ComponentWave()
{
}

void ComponentWave::SetWaveType(int type)
{
  m_waveType = type;
}

void ComponentWave::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();

  double freq = 1.0, amp = 1.0, phaseMod = 0.0;
  GetInput(0, &freq);
  GetInput(1, &amp);
  GetInput(2, &phaseMod);

  double v = 0.0;
  if (m_waveType == CW_SINE)
    v = amp * sinf((m_phase / 48000) * M_PI * 2 + phaseMod);
  else if (m_waveType == CW_SQUARE)
    v = amp * copysign(1.0, sinf((m_phase / 48000) * M_PI * 2 + phaseMod));
  else if (m_waveType == CW_SAWTOOTH)
    v = amp * 2.0 * (m_phase / 48000 - std::floor(m_phase / 48000) - 0.5);
  m_phase += freq;
  if (m_phase > 48000)
    m_phase -= 48000;

  SetOutput(0, v);
}

void ComponentWave::OnRightClicked()
{
  rwContextMenu *cm = new rwContextMenu(Component::m_position,
    rwFUNCTOR(OnSelection), this);
  cm->AddChoice("Sine", (void *)CW_SINE);
  cm->AddChoice("Square", (void *)CW_SQUARE);
  cm->AddChoice("Sawtooth", (void *)CW_SAWTOOTH);
}

void ComponentWave::OnSelection(ComponentWave *caller, rwContextMenuEvent &e)
{
  if (e.type == rwCONTEXT_SELECT)
    caller->SetWaveType((int)e.data);
}

std::string ComponentWave::GetNodeType()
{
  double freq = 1.0, amp = 1.0;
  if (m_inputs[0].comp != NULL)
    freq = *(double *)m_inputs[0].value;
  if (m_inputs[1].comp != NULL)
    amp = *(double *)m_inputs[1].value;

  std::string waveType = "Wave";
  if      (m_waveType == CW_SINE)     waveType = "Sine";
  else if (m_waveType == CW_SQUARE)   waveType = "Square";
  else if (m_waveType == CW_SAWTOOTH) waveType = "Sawtooth";

  std::stringstream t;
  t << waveType << " (f = " << freq << ", a = " << amp << ")";

  return t.str();
}

void ComponentWave::ToFile(std::ostream &out)
{
  out << "Wave";
  WriteDefaultParams(out);
  out << m_waveType;
}

Component *ComponentWave::FromFile(std::istream &in)
{
  ComponentWave *c = new ComponentWave();
  c->ReadDefaultParams(in);
  int type;
  in >> type;
  c->SetWaveType(type);

  return c;
}

Component *ComponentWave::Create()
{
  return new ComponentWave();
}

Component *ComponentWave::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
