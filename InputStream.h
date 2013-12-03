
#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include "AudioStream.h"

class InputStream: public AudioStream
{
  protected:
  void Initialize();
  static int Callback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *data);

  public:
  InputStream(pthread_mutex_t *mutex, PaStreamCallback callback, void *data);
  ~InputStream();

  void SetDevice(int ind);
};

#endif
