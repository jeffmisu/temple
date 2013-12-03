
#include "WAV.h"

WAV::WAV()
{
}

WAV::WAV(std::string filename)
{
  LoadFromFile(filename);
}

WAV::~WAV()
{
}

void WAV::LoadFromFile(std::string filename)
{
	PSF_PROPS props;
  int ifd = psf_sndOpen(filename.c_str(), &props, 0);

  if (ifd < 0)
  {
    m_samples.clear();
    return;
  }

  int framesread;
  int totalread;

  // Allocate space for the frames
  m_channelCount = props.chans;
  m_samples.resize(psf_sndSize(ifd) * m_channelCount);

  float *frame = &m_samples[0];

  framesread = psf_sndReadFloatFrames(ifd, frame, 1);
  totalread = 0;
  while (framesread == 1)
  {
    totalread++;
    frame += m_channelCount;
    framesread = psf_sndReadFloatFrames(ifd, frame, 1);
  }
}

int WAV::GetSampleCount() const
{
  return m_samples.size() / m_channelCount;
}

float WAV::GetSample(int index, int channel) const
{
  return m_samples[index * m_channelCount + channel];
}
