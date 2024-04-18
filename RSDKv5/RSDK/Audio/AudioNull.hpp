#ifndef AUDIO_H
#define AUDIO_H

namespace RSDK
{

#define SFX_COUNT     (0)
#define CHANNEL_COUNT (0)

#define MIX_BUFFER_SIZE (0)
#define SAMPLE_FORMAT   float

#define AUDIO_FREQUENCY (0)
#define AUDIO_CHANNELS  (0)
struct ChannelInfo {
    float *samplePtr;
    float pan;
    float volume;
    int32 speed;
    size_t sampleLength;
    int32 bufferPos;
    int32 playIndex;
    uint32 loop;
    int16 soundID;
    uint8 priority;
    uint8 state;
};

namespace AudioDevice
{
    inline bool32 Init(){
        return true;
    }

    inline void Release(){
        return;
    }

    inline void ProcessAudioMixing(void *stream, int32 length){
        return;
    }

    inline void FrameInit(){
        return;
    }

    inline void HandleStreamLoad(ChannelInfo *channel, bool32 async){
        return;
    }

    static uint8 initializedAudioChannels = true;
    static uint8 audioState = true;
    static uint8 audioFocus = 0;
}

inline uint16 GetSfx(const char *sfxName)
{
    return -1;
}

inline int32 PlaySfx(uint16 sfx, uint32 loopPoint, uint32 priority)
{
    return -1;
}

inline void StopSfx(uint16 sfx)
{
    return;
}

#if RETRO_REV0U
inline void StopAllSfx()
{
    return;
}
#endif

inline int32 PlayStream(const char *filename, uint32 slot, uint32 startPos, uint32 loopPoint, bool32 loadASync)
{
        return -1;
}

inline void SetChannelAttributes(uint8 channel, float volume, float panning, float speed)
{
    return;
}

inline void StopChannel(uint32 channel)
{
    return;
}

inline void PauseChannel(uint32 channel)
{
    return;
}

inline void ResumeChannel(uint32 channel)
{
    return;
}

inline void PauseSound()
{
    return;
}

inline bool32 SfxPlaying(uint16 sfx)
{
    return false;
}

inline void ResumeSound()
{
    return;
}

inline bool32 IsSfxPlaying(uint32 channel)
{
        return false;
}

inline bool32 ChannelActive(uint32 channel)
{
        return false;
}

inline uint32 GetChannelPos(uint32 channel)
{
    return 0;
}
inline double GetVideoStreamPos()
{
    return -1.0;
}

inline void LoadSfxToSlot(char *filename, uint8 slot, uint8 plays, uint8 scope)
{
    return;
}
inline void LoadSfx(char *filePath, uint8 plays, uint8 scope)
{
    return;
}

inline void ClearStageSfx(){
    return;
}

#if RETRO_REV0U
#include "Legacy/AudioLegacy.hpp"
#endif

} // namespace RSDK

#endif
