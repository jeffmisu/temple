
#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <portaudio.h>
#include <pthread.h>
#include <vector>
#include <string>

class ComponentBoard;

class AudioStream
{
  protected:
  PaStreamParameters m_params;
  PaStream *m_stream;
  pthread_mutex_t *m_mutex;
  std::string m_devName;
  int m_devIndex;

  int m_framesPerBuffer;
  int m_numChannels;
  int m_sampleRate;

  PaStreamCallback *m_callback;
  void *m_data;

  public:
  AudioStream(pthread_mutex_t *mutex, PaStreamCallback callback, void *data);
  virtual ~AudioStream();

  int GetFramesPerBuffer() const;
  int GetNumChannels() const;
  int GetSampleRate() const;
  std::string GetDeviceName() const;
  int GetDeviceIndex() const;

  int GetDeviceCount() const;
  std::vector<std::string> GetDeviceNames() const;

  void SetMutex(pthread_mutex_t *mutex);
  void LockMutex();
  void UnlockMutex();
};

#endif
