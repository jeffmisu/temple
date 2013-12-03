
#ifndef COMPONENTOUTPUTCHANNEL_H
#define COMPONENTOUTPUTCHANNEL_H

#include "../../Component.h"
#include "../../OutputStream.h"

//CMP_NAME=Output channel

class ComponentOutputChannel: public Component
{
  protected:
  OutputStream *m_stream;

  static int Callback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *data);

  public:
  ComponentOutputChannel();
  ~ComponentOutputChannel();

  void Evaluate(int timestamp);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
