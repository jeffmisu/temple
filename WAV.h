
#ifndef WAV_H
#define WAV_H

#include <portsf.h>
#include <vector>
#include <string>

class WAV
{
  protected:
  std::vector<float> m_samples;
  int m_channelCount;

  public:
  WAV();
  WAV(std::string filename);
  ~WAV();

  void LoadFromFile(std::string filename);

  int GetSampleCount() const;
  float GetSample(int index, int channel = 0) const;
};

#endif
