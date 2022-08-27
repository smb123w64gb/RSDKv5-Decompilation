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

void AudioThread(void* arg);

inline s32 GetThreadPriority() {
  s32 prio = 0x30;
  svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
  prio -= 1;
  prio = prio < 0x18 ? 0x18 : prio;
  prio = prio > 0x3f ? 0x3f : prio;

  return prio;
}

bool32 AudioDevice::Init()
{
  ndspChnReset(0);
  ndspSetOutputMode(NDSP_OUTPUT_STEREO);
  ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
  ndspChnSetRate(0, SAMPLE_RATE);
  ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

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
  // TODO: run thread on different core depending on model?
  s32 prio = GetThreadPriority();
  threadRunning = true;
  audioThreadHandle = threadCreate(AudioThread, 0,
                                          THREAD_STACK_SZ, prio,
                                          -1, false);
  if (!audioThreadHandle) {
    PrintLog(PRINT_NORMAL, "ERROR: failed to init audio thread\n");
  }

  // create event for audio thread 
  svcCreateEvent(&audioThreadRequest, 0);
    
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
  // TODO: everything
  return;
}

void AudioDevice::FrameInit() {
  // TODO: everything
  return;
}

void AudioDevice::HandleStreamLoad(ChannelInfo* channel, bool32 async)
{
  // TODO: support async at some point, maybe?
  //LoadStream(channel);
}

void AudioThread(void* arg) {
  PrintLog(PRINT_NORMAL, "CTRAudioDevice: audio thread created\n");
  while (threadRunning) {
    // TODO: have thread actually do stuff
    svcWaitSynchronization(audioThreadRequest, UINT64_MAX);
    svcClearEvent(audioThreadRequest);
  }
}
