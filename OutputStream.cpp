
#include "OutputStream.h"

#include <iostream>
#include <string>

OutputStream::OutputStream(pthread_mutex_t *mutex, PaStreamCallback callback,
  void *data):
  AudioStream(mutex, callback, data)
{
  Initialize();
}

OutputStream::~OutputStream()
{
  Pa_AbortStream(m_stream);
  Pa_CloseStream(m_stream);
}

void OutputStream::Initialize()
{
  PaError err = Pa_Initialize();
  if (err != paNoError) goto error;

  m_params.device = Pa_GetDefaultOutputDevice();
  m_devName = Pa_GetDeviceInfo(m_params.device)->name;
  m_devIndex = m_params.device;
  std::cout << "Output device #" << m_params.device << std::endl;
  m_params.channelCount = 2;
  m_params.sampleFormat = paFloat32;
  m_params.suggestedLatency = 0.0;
  // consider: Pa_GetDeviceInfo(m_params.device)->defaultLowOutputLatency;
  m_params.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
    &m_stream,
    NULL,
    &m_params,
    m_sampleRate,
    m_framesPerBuffer,
    paClipOff,
    m_callback,
    m_data);  // User-specified callback parameter
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

int OutputStream::Callback(const void *inputBuffer, void *outputBuffer,
  unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags, void *data)
{
  OutputStream *caller = (OutputStream *)data;
  float *out = (float *)outputBuffer;

  caller->LockMutex();

  // Fill the output buffer
  for (int i = 0; i < caller->m_framesPerBuffer; ++i)
  {
    double v = 0.0;

    if (v > 1.0)
      v = 1.0;
    else if (v < -1.0)
      v = -1.0;
    *out++ = v; // Left channel
    *out++ = v; // Right channel
  }

  caller->UnlockMutex();

  return paContinue;
}

void OutputStream::StartStream()
{
  const PaStreamInfo *si;
  PaError err = Pa_StartStream(m_stream);
  if (err != paNoError) goto error;

  si = Pa_GetStreamInfo(m_stream);
  std::cout << "Output stream latency: " << si->outputLatency << std::endl;

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
