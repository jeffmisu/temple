
#include "ComponentOutputChannel.h"

#include <sstream>
#include <fstream>

const sPatchIn blueprint[] =
{
  sPatchIn(CT_DOUBLE, "Right channel"),
  sPatchIn(CT_DOUBLE, "Left channel")
};

ComponentOutputChannel::ComponentOutputChannel()
{
  InitializePatches(blueprint, sizeof(blueprint) / sizeof(sPatchIn), NULL, 0);

  m_stream = new OutputStream(NULL, Callback, this);
  m_stream->StartStream();
}

ComponentOutputChannel::~ComponentOutputChannel()
{
  delete m_stream;
}

int ComponentOutputChannel::Callback(const void *inputBuffer,
  void *outputBuffer, unsigned long framesPerBuffer,
  const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
  void *data)
{
  ComponentOutputChannel *caller = (ComponentOutputChannel *)data;
  if (caller == NULL)
    return paContinue;

  float *out = (float *)outputBuffer;

  caller->m_stream->LockMutex();

  // Fill the output buffer
  // I'm breaking the cases up to move the conditional outside the loop,
  // because it makes me feel better about performance
  if (caller->m_inputs[0].value != NULL && caller->m_inputs[1].value != NULL)
  {
    for (unsigned int i = 0; i < framesPerBuffer; ++i)
    {
      caller->Evaluate(0);
      double vR = *(double *)caller->m_inputs[0].value;
      double vL = *(double *)caller->m_inputs[1].value;

      if (vR > 1.0) vR = 1.0;
      else if (vR < -1.0) vR = -1.0;
      if (vL > 1.0) vL = 1.0;
      else if (vL < -1.0) vL = -1.0;

      *out++ = vL; // Left channel
      *out++ = vR; // Right channel
    }
  }
  else if (caller->m_inputs[0].value != NULL)
  {
    for (unsigned int i = 0; i < framesPerBuffer; ++i)
    {
      caller->Evaluate(0);
      double vR = *(double *)caller->m_inputs[0].value;

      if (vR > 1.0) vR = 1.0;
      else if (vR < -1.0) vR = -1.0;

      *out++ = 0.; // Left channel
      *out++ = vR; // Right channel
    }
  }
  else if (caller->m_inputs[1].value != NULL)
  {
    for (unsigned int i = 0; i < framesPerBuffer; ++i)
    {
      caller->Evaluate(0);
      double vL = *(double *)caller->m_inputs[1].value;

      if (vL > 1.0) vL = 1.0;
      else if (vL < -1.0) vL = -1.0;

      *out++ = vL; // Left channel
      *out++ = 0.; // Right channel
    }
  }

  caller->m_stream->UnlockMutex();

  return paContinue;
}

void ComponentOutputChannel::Evaluate(int timestamp)
{
  if (m_inputs[0].comp != NULL)
    m_inputs[0].comp->Evaluate(
      ADVANCE_TIMESTAMP(m_inputs[0].comp->GetTimestamp()));
  if (m_inputs[1].comp != NULL && m_inputs[1].comp != m_inputs[0].comp)
    m_inputs[1].comp->Evaluate(
      ADVANCE_TIMESTAMP(m_inputs[1].comp->GetTimestamp()));
}

std::string ComponentOutputChannel::GetNodeType()
{
  std::stringstream t;
  t << "Output (" <<
    ((m_inputs[0].comp == NULL)?(0.0):(*(double *)m_inputs[0].value)) << ")";

  return t.str();
}

void ComponentOutputChannel::ToFile(std::ostream &out)
{
  out << "OutputChannel";
  WriteDefaultParams(out);
}

Component *ComponentOutputChannel::FromFile(std::istream &in)
{
  ComponentOutputChannel *c = new ComponentOutputChannel();
  c->ReadDefaultParams(in);

  return c;
}

Component *ComponentOutputChannel::Create()
{
  return new ComponentOutputChannel();
}

Component *ComponentOutputChannel::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
