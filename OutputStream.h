
#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include "AudioStream.h"

class OutputStream: public AudioStream
{
  protected:
  void Initialize();
  static int Callback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *data);

  public:
  OutputStream(pthread_mutex_t *mutex, PaStreamCallback callback, void *data);
  ~OutputStream();

  void StartStream();
};

#endif
