
#include "InputStream.h"

#include <iostream>
#include <string>

InputStream::InputStream(pthread_mutex_t *mutex, PaStreamCallback callback,
  void *data):
  AudioStream(mutex, callback, data)
{
  Initialize();
}

InputStream::~InputStream()
{
  Pa_AbortStream(m_stream);
  Pa_CloseStream(m_stream);
}

void InputStream::Initialize()
{
  PaError err = Pa_Initialize();
  if (err != paNoError) goto error;

  SetDevice(Pa_GetDefaultInputDevice());

error:
  if (m_stream)
  {
    Pa_AbortStream(m_stream);
    Pa_CloseStream(m_stream);
  }
  std::cout << "Encountered an error when opening/starting a stream. (" << err
    << ")" << std::endl;
  Pa_Terminate();
}

int InputStream::Callback(const void *inputBuffer, void *ouputBuffer,
  unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags, void *data)
{
  InputStream *caller = (InputStream *)data;
  float *in = (float *)inputBuffer;

  caller->LockMutex();

  // Use the input buffer
  *in = 0;

  caller->UnlockMutex();

  return paContinue;
}

void InputStream::SetDevice(int ind)
{
  Pa_AbortStream(m_stream);
  Pa_CloseStream(m_stream);

  m_params.device = ind;
  m_devName = Pa_GetDeviceInfo(ind)->name;
  m_devIndex = ind;
  std::cout << "Input device #" << m_params.device << std::endl;
  m_params.channelCount = 2;
  m_params.sampleFormat = paFloat32;
  m_params.suggestedLatency = 0.01;
  m_params.hostApiSpecificStreamInfo = NULL;

  PaError err = Pa_OpenStream(
    &m_stream,
    &m_params,
    NULL,
    m_sampleRate,
    m_framesPerBuffer,
    paClipOff,
    m_callback,
    m_data);  // User-specified callback parameter
  if (err != paNoError) goto error;

  err = Pa_StartStream(m_stream);
  if (err != paNoError) goto error;

  return;

error:
  if (m_stream)
  {
     Pa_AbortStream(m_stream);
     Pa_CloseStream(m_stream);
  }
  std::cout << "Encountered an error when opening/starting a stream. (" << err
    << ")" << std::endl;
  Pa_Terminate();
}
