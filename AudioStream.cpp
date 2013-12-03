
#include "AudioStream.h"

#include <iostream>
#include <string>

AudioStream::AudioStream(pthread_mutex_t *mutex, PaStreamCallback callback,
  void *data):
  m_stream(NULL),
  m_mutex(mutex),
  m_devName("None"),
  m_devIndex(-1),
  m_framesPerBuffer(2000),
  m_numChannels(2),
  m_sampleRate(48000),
  m_callback(callback),
  m_data(data)
{
}

AudioStream::~AudioStream()
{
  PaError err = Pa_StopStream(m_stream);
  if (err != paNoError)
  {
    if (m_stream)
    {
       Pa_AbortStream(m_stream);
       Pa_CloseStream(m_stream);
    }
    std::cout << "Encountered an error while closing a stream. (" << err << ")"
      << std::endl;
  }
}

int AudioStream::GetFramesPerBuffer() const
{
  return m_framesPerBuffer;
}

int AudioStream::GetNumChannels() const
{
  return m_numChannels;
}

int AudioStream::GetSampleRate() const
{
  return m_sampleRate;
}

std::string AudioStream::GetDeviceName() const
{
  return m_devName;
}

int AudioStream::GetDeviceIndex() const
{
  return m_devIndex;
}

int AudioStream::GetDeviceCount() const
{
  return Pa_GetDeviceCount();
}

std::vector<std::string> AudioStream::GetDeviceNames() const
{
  int devct = Pa_GetDeviceCount();
  std::vector<std::string> names(devct);
  for (int i = 0; i < devct; i++)
  {
    const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
    names[i] = info->name;
  }

  return names;
}

void AudioStream::SetMutex(pthread_mutex_t *mutex)
{
  m_mutex = mutex;
}

void AudioStream::LockMutex()
{
  if (m_mutex != NULL)
    pthread_mutex_lock(m_mutex);
}

void AudioStream::UnlockMutex()
{
  if (m_mutex != NULL)
    pthread_mutex_unlock(m_mutex);
}

/*
pthread_mutex_t synthMutex;

PaStreamParameters inputParameters, outputParameters;
PaStream *stream = NULL;
PaError err;
char *sampleBlock;
int numBytes;

static int audioCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
  const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
  float *out = (float *)outputBuffer;
  float *in = (float *)inputBuffer;

  pthread_mutex_lock(&synthMutex);

  for (unsigned int i = 0; i < framesPerBuffer; ++i)
  {
    double v = 0.0;

    if (userData != NULL)
      v = 0;//((ComponentBoard *)userData)->Evaluate();
    if (v > 1.0)
      v = 1.0;
    else if (v < -1.0)
      v = -1.0;
    *out++ = v; // Left channel
    *out++ = v; // Right channel
  }

  pthread_mutex_unlock(&synthMutex);

  //err = Pa_ReadStream( stream, sampleBlock, FRAMES_PER_BUFFER );
  //err = Pa_WriteStream( stream, sampleBlock, FRAMES_PER_BUFFER );

  return 0;
}

int audioInit(void *generator)
{
  numBytes = FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE ;
  sampleBlock = (char *) malloc( numBytes );
  if( sampleBlock == NULL )
  {
      printf("Could not allocate record array.\n");
      exit(1);
  }
  CLEAR( sampleBlock );

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  inputParameters.device = Pa_GetDefaultInputDevice();
  printf( "Input device # %d.\n", inputParameters.device );
  printf( "Input LL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency );
  printf( "Input HL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency );
  inputParameters.channelCount = NUM_CHANNELS;
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  // Setup

 err = Pa_OpenStream(
            &stream,
            &inputParameters,
            NULL,//&outputParameters,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            paClipOff,
            audioCallback,
            generator);
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  return 0;

error:
  if( stream ) {
     Pa_AbortStream( stream );
     Pa_CloseStream( stream );
  }
  free( sampleBlock );
  Pa_Terminate();
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
  return -1;
}

int audioEnd()
{
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    CLEAR( sampleBlock );

    free( sampleBlock );

    Pa_Terminate();
    return 0;

error:
    if( stream ) {
       Pa_AbortStream( stream );
       Pa_CloseStream( stream );
    }
    free( sampleBlock );
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}
*/
