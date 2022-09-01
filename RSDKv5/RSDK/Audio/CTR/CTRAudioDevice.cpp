using namespace RSDK;

// heavily referenced:  https://github.com/devkitPro/3ds-examples/blob/master/audio/opus-decoding/source/main.c

#define SAMPLE_RATE         (AUDIO_FREQUENCY)
#define SAMPLES_PER_BUF     (SAMPLE_RATE * 120 / 1000)
#define CHANNELS_PER_SAMPLE (2)
#define THREAD_STACK_SZ     (32 * 1024)
#define WAVEBUF_COUNT       (3)

static const int WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE *
                                sizeof(int16);

Thread audioThreadHandle;
Handle audioThreadRequest;

volatile bool threadRunning;

ndspWaveBuf wbuf[WAVEBUF_COUNT];
int16* audioBuffer = NULL;

void AudioCallback(void *const nul_) {
  (void) nul_;

  if (!threadRunning)
    return;

  svcSignalEvent(audioThreadRequest);
}

void AudioThread(void* arg);

inline s32 GetThreadPriority() {
  s32 prio = 0x30;
  svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
  prio -= 1;
  prio = prio < 0x18 ? 0x18 : prio;
  prio = prio > 0x3f ? 0x3f : prio;

  return prio;
}

inline u16 convertSample(float f) {
  float fOut = (f * 65535);
  u16 out;
  if (fOut < 0) out = 0;
  else if (fOut > 65535) out = 65535;
  else out = (u16) fOut;
  return out;
}

bool32 AudioDevice::Init()
{
  // taken from 3DS audio example
  ndspInit();
  ndspChnReset(0);
  ndspSetOutputMode(NDSP_OUTPUT_STEREO);
  ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
  ndspChnSetRate(0, SAMPLE_RATE);
  ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

  InitAudioChannels();

  
  // allocate space for audio buffer
  const size_t bufferSize = WAVEBUF_SIZE * WAVEBUF_COUNT;
  audioBuffer = (int16*) linearAlloc(bufferSize);
  if (!audioBuffer) {
    PrintLog(PRINT_NORMAL, "ERROR: failed to allocate audio buffer\n");
    return false;
  }

  // set up ndsp wavebufs
  memset(&wbuf, 0, sizeof(wbuf));
  int16* buf = audioBuffer;
  for (int i = 0; i < WAVEBUF_COUNT; i++) {
    wbuf[i].data_vaddr = buf;
    wbuf[i].status     = NDSP_WBUF_DONE;

    buf += WAVEBUF_SIZE / sizeof(buf[0]);
  }

  // create audio thread
  // (run on same core as main thread)
  s32 prio = GetThreadPriority();
  threadRunning = true;
  audioThreadHandle = threadCreate(AudioThread, 0,
                                          THREAD_STACK_SZ, prio,
                                          0, false);
  if (!audioThreadHandle) {
    PrintLog(PRINT_NORMAL, "ERROR: failed to init audio thread\n");
  }

  // create event for audio thread 
  svcCreateEvent(&audioThreadRequest, 0);
  
  // set audio callback (simply triggers audioThreadRequest event)
  ndspSetCallback(AudioCallback, NULL);
  
  return true;
}

void AudioDevice::Release()
{
  // close thread
  threadRunning = false;
  threadJoin(audioThreadHandle, UINT64_MAX);
  threadFree(audioThreadHandle);

  // close event handle 
  svcCloseHandle(audioThreadRequest);

  // ... then de-init audio stuff 
  ndspChnReset(0);
  linearFree(audioBuffer);
  ndspExit();
}

void AudioDevice::ProcessAudioMixing(void* stream, int32 length)
{
  // taken from SDL2 backend
  u16 *streamF    = (u16*)stream;
  u16 *streamEndF = ((u16*)stream) + length;

  memset(stream, 0, length * sizeof(SAMPLE_FORMAT));

  for (int32 c = 0; c < CHANNEL_COUNT; ++c) {
      ChannelInfo *channel = &channels[c];

      switch (channel->state) {
          default:
          case CHANNEL_IDLE: break;

          case CHANNEL_SFX: {
              SAMPLE_FORMAT *sfxBuffer = &channel->samplePtr[channel->bufferPos];

              // somehow it can get here and not have any data to play, causing a crash. This should fix that
              if (!sfxBuffer) {
                  channel->state   = CHANNEL_IDLE;
                  channel->soundID = -1;
                  continue;
              }

              float volL = channel->volume, volR = channel->volume;
              if (channel->pan < 0.0)
                  volL = (1.0 + channel->pan) * channel->volume;
              else
                  volR = (1.0 - channel->pan) * channel->volume;

              float panL = volL * engine.soundFXVolume;
              float panR = volR * engine.soundFXVolume;

              uint32 speedPercent       = 0;
              u16 *curStreamF           = streamF;
              while (curStreamF < streamEndF && streamF < streamEndF) {
                  SAMPLE_FORMAT sample = (sfxBuffer[1] - *sfxBuffer) * speedMixAmounts[speedPercent >> 6] + *sfxBuffer;

                  speedPercent += channel->speed;
                  sfxBuffer += FROM_FIXED(speedPercent);
                  channel->bufferPos += FROM_FIXED(speedPercent);
                  speedPercent &= 0xFFFF;

                  curStreamF[0] += convertSample(sample * panR);
                  curStreamF[1] += convertSample(sample * panL);
                  curStreamF += 2;

                  if (channel->bufferPos >= channel->sampleLength) {
                      if (channel->loop == 0xFFFFFFFF) {
                          channel->state   = CHANNEL_IDLE;
                          channel->soundID = -1;
                          break;
                      }
                      else {
                          channel->bufferPos -= channel->sampleLength;
                          channel->bufferPos += channel->loop;

                          sfxBuffer = &channel->samplePtr[channel->bufferPos];
                      }
                  }
              }

              break;
          }

          case CHANNEL_STREAM: {
              SAMPLE_FORMAT *streamBuffer = &channel->samplePtr[channel->bufferPos];

              // somehow it can get here and not have any data to play, causing a crash. This should fix that
              if (!streamBuffer) {
                  channel->state   = CHANNEL_IDLE;
                  channel->soundID = -1;
                  continue;
              }

              float volL = channel->volume, volR = channel->volume;
              if (channel->pan < 0.0)
                  volL = (1.0 + channel->pan) * channel->volume;
              else
                  volR = (1.0 - channel->pan) * channel->volume;

              float panL = volL * engine.streamVolume;
              float panR = volR * engine.streamVolume;

              uint32 speedPercent       = 0;
              u16 *curStreamF           = streamF;
              while (curStreamF < streamEndF && streamF < streamEndF) {
                  speedPercent += channel->speed;
                  int32 next = FROM_FIXED(speedPercent);
                  speedPercent &= 0xFFFF;

                  curStreamF[0] += convertSample(panR * *streamBuffer);
                  curStreamF[1] += convertSample(panL * streamBuffer[next]);
                  curStreamF += 2;

                  streamBuffer += next * 2;
                  channel->bufferPos += next * 2;

                  if (channel->bufferPos >= channel->sampleLength) {
                      channel->bufferPos -= channel->sampleLength;

                      streamBuffer = &channel->samplePtr[channel->bufferPos];

                      UpdateStreamBuffer(channel);
                  }
              }
              break;
          }

          case CHANNEL_LOADING_STREAM: break;
      }
  }
}

void AudioDevice::FrameInit() {
  //svcSignalEvent(audioThreadRequest);
}

void AudioDevice::HandleStreamLoad(ChannelInfo* channel, bool32 async)
{
  // TODO: support async at some point, maybe?
  LoadStream(channel);
}

void AudioDevice::InitAudioChannels() {
  // taken from SDL2 backend
  for (int32 i = 0; i < CHANNEL_COUNT; i++) {
    channels[i].soundID = -1;
    channels[i].state = CHANNEL_IDLE;
  }

  for (int32 i = 0; i < 0x400; i += 2) {
    speedMixAmounts[i]     = (i + 0) * (1.0f / 1024.0f);
    speedMixAmounts[i + 1] = (i + 1) * (1.0f / 1024.0f);
  }

  GEN_HASH_MD5("Stream Channel 0", sfxList[SFX_COUNT - 1].hash);
  sfxList[SFX_COUNT - 1].scope              = SCOPE_GLOBAL;
  sfxList[SFX_COUNT - 1].maxConcurrentPlays = 1;
  sfxList[SFX_COUNT - 1].length             = MIX_BUFFER_SIZE;
  AllocateStorage((void**)&sfxList[SFX_COUNT - 1].buffer,
                  MIX_BUFFER_SIZE * sizeof(SAMPLE_FORMAT),
                  DATASET_MUS,
                  false);
}

void AudioThread(void* arg) {
  PrintLog(PRINT_NORMAL, "CTRAudioDevice: audio thread created\n");

  while (threadRunning) {
    for (size_t i = 0; i < WAVEBUF_COUNT; i++) {
      svcWaitSynchronization(audioThreadRequest, UINT64_MAX);
      svcClearEvent(audioThreadRequest);

      if (wbuf[i].status != NDSP_WBUF_DONE)
        continue;

      AudioDevice::ProcessAudioMixing(wbuf[i].data_pcm16, 
                                      WAVEBUF_SIZE);
      wbuf[i].nsamples = SAMPLES_PER_BUF;
      ndspChnWaveBufAdd(0, &wbuf[i]);
      DSP_FlushDataCache(wbuf[i].data_pcm16,
                          SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE * 
                          sizeof(int16_t));
    }
  }
}
