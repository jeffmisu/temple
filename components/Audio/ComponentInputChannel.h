
#ifndef COMPONENTINPUTCHANNEL_H
#define COMPONENTINPUTCHANNEL_H

#include "../../Component.h"
#include "../../InputStream.h"

//CMP_NAME=Input Channel

class ComponentInputChannel: public Component
{
  protected:
  InputStream *m_stream;
  std::vector<float> m_samples;
  int m_indWrite;
  int m_indRead;

  static int Callback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *data);

  void WriteSample(float s);
  float ReadSample();

  public:
  ComponentInputChannel();
  ~ComponentInputChannel();

  void Evaluate(int timestamp);

  void OnRightClicked();
  static void OnSelection(ComponentInputChannel *caller,
    rwContextMenuEvent &e);

  std::string GetNodeType();

  void ToFile(std::ostream &out);
  static Component *FromFile(std::istream &in);
  static Component *Create();
  Component *Copy();
};

#endif
