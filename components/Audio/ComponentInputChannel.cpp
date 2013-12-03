
#include "ComponentInputChannel.h"

#include <sstream>
#include <fstream>

ComponentInputChannel::ComponentInputChannel():
  m_indWrite(0),
  m_indRead(0)
{
  InitializePatches(NULL, 0, CT_DOUBLE);

  m_stream = new InputStream(NULL, Callback, this);
  m_samples.resize(m_stream->GetFramesPerBuffer() * 100);
}

ComponentInputChannel::~ComponentInputChannel()
{
  delete m_stream;
}

int ComponentInputChannel::Callback(const void *inputBuffer,
  void *outputBuffer, unsigned long framesPerBuffer,
  const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
  void *data)
{
  ComponentInputChannel *caller = (ComponentInputChannel *)data;
  float *in = (float *)inputBuffer;

  //caller->m_stream->LockMutex();

  for (unsigned int i = 0; i < framesPerBuffer; i++)
    caller->WriteSample(in[i * caller->m_stream->GetNumChannels()]);

  //caller->m_stream->UnlockMutex();

  return paContinue;
}

void ComponentInputChannel::WriteSample(float s)
{
  m_samples[m_indWrite] = s;
  m_indWrite = (m_indWrite + 1) % m_samples.size();
}

float ComponentInputChannel::ReadSample()
{
  float s = m_samples[m_indRead];
  if (m_indRead != m_indWrite)
    m_indRead = (m_indRead + 1) % m_samples.size();

  return s;
}

void ComponentInputChannel::Evaluate(int timestamp)
{
  UPDATE_TIMESTAMP();
  SetOutput(0, ReadSample());
/*
  m_value = m_samples[m_indRead];
  m_indRead += 2;

  if (m_indRead >= m_samples.size())
    m_indRead = 0;

  return m_value;
*/
}

void ComponentInputChannel::OnRightClicked()
{
  rwContextMenu *cm = new rwContextMenu(Component::m_position,
    rwFUNCTOR(OnSelection), this);
  std::vector<std::string> devnames = m_stream->GetDeviceNames();

  for (size_t i = 0; i < devnames.size(); i++)
    cm->AddChoice(devnames[i], (void *)i);
}

void ComponentInputChannel::OnSelection(ComponentInputChannel *caller,
  rwContextMenuEvent &e)
{
  if (e.type == rwCONTEXT_SELECT)
    caller->m_stream->SetDevice((int)e.data);
}

std::string ComponentInputChannel::GetNodeType()
{
  std::stringstream t;
  t << "Input (" << m_stream->GetDeviceName() << ")";

  return t.str();
}

void ComponentInputChannel::ToFile(std::ostream &out)
{
  out << "InputChannel";
  WriteDefaultParams(out);
  out << m_stream->GetDeviceIndex();
}

Component *ComponentInputChannel::FromFile(std::istream &in)
{
  ComponentInputChannel *c = new ComponentInputChannel();
  c->ReadDefaultParams(in);
  int index;
  in >> index;
  c->m_stream->SetDevice(index);

  return c;
}

Component *ComponentInputChannel::Create()
{
  return new ComponentInputChannel();
}

Component *ComponentInputChannel::Copy()
{
  std::stringstream t;
  ToFile(t);
  return FromFile(t);
}
