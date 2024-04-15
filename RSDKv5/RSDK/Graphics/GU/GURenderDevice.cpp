
#define SCREEN_XSIZE (480)
#define SCREEN_YSIZE (272)
#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
using namespace RSDK;

bool RenderDevice::Init()
{
    sceGuInit();
    sceGuDrawBuffer(GU_PSM_5650,(void*)0,512);
    sceGuDispBuffer(SCREEN_XSIZE,SCREEN_YSIZE,(void*)0x44000,512);

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
  // dummy out later maybe possibly
  printf("RSDKv5 PSP: RenderDevice init\n");
  printf("The programmer has a nap. Hold out!\nProgrammer!\n");


  scanlines = (ScanlineInfo*) malloc(SCREEN_YSIZE * sizeof(ScanlineInfo));
  if (!scanlines)
    return false;

  engine.inFocus = 1;
  videoSettings.windowState = WINDOWSTATE_ACTIVE;
  videoSettings.dimMax = 1.0;
  videoSettings.dimPercent = 1.0;

  RSDK::SetScreenSize(0, SCREEN_XSIZE, SCREEN_YSIZE);

  memset(screens[0].frameBuffer, 0, SCREEN_XSIZE * SCREEN_YSIZE * sizeof(uint16));

  InitInputDevices();
  if (!AudioDevice::Init())
    return false;

  return true;
}

void RenderDevice::CopyFrameBuffer()
{
  return;
}

// https://github.com/JeffRuLz/Sonic-1-2-2013-Decompilation/blob/main/RSDKv4/3ds/3ds.cpp#L141
void RenderDevice::FlipScreen()
{
  //u16 *src = screens[0].frameBuffer;
  //u16 *dst = (uint16*) gfxGetFramebuffer(GFX_TOP, GFX_LEFT, 0, 0);

  switch (videoSettings.screenCount) {
    default:
    case 0:
      // image/video buffer, break
      break;
    case 1:
    //sceGuCopyImage(GU_PSM_5650,0,0,480,272,512,screens[0].frameBuffer,0,0,512,(void*)(((unsigned int)framebuffer)+0x4000000));
      /*for (int y = 0; y < SCREEN_YSIZE; y++) {
        for (int x = 0; x < SCREEN_XSIZE; x++) {
          dst[((x * 240) + (240 - y - 1))] = *src++;
        }
      }*/
      break;

    // the separate screens used for competition mode will likely 
    // never be supported by the 3DS port
    case 2:
    case 3:
    case 4:
      break;
  }


  /*
  if (windowRefreshDelay > 0) {
    windowRefreshDelay--;
    if (!windowRefreshDelay)
      UpdateGameWindow();
  }
  */
}

void RenderDevice::Release(bool32 isRefresh)
{
  if (scanlines)
    free(scanlines);

  if (!isRefresh) {
    //gfxExit();
  }
}

void RenderDevice::RefreshWindow()
{

}

void RenderDevice::SetupImageTexture(int32 width, int32 height, uint8* imagePixels)
{
  // TODO: implement
  return;
}

// TODO: you may have to rewrite parts of the engine elsewhere to hack 
// 3ds-theoraplayer in, just leave like this for now
void RenderDevice::SetupVideoTexture_YUV420(int32 width, int32 height, uint8* imagePixels)
{
  return;
}

void RenderDevice::SetupVideoTexture_YUV422(int32 width, int32 height, uint8* imagePixels)
{
  return;
}

void RenderDevice::SetupVideoTexture_YUV424(int32 width, int32 height, uint8* imagePixels)
{
  return;
}

bool RenderDevice::ProcessEvents()
{
  return true;
}

// TODO: re-use the frame limiter @JeffRuLz implemented with the CD port
void RenderDevice::InitFPSCap()
{
  return;
}

bool RenderDevice::CheckFPSCap()
{
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
    return true;
}

void RenderDevice::UpdateFPSCap()
{
  return;
}

// NOTE: shaders likely won't ever be supported by the 3DS port; given
// that the Retro Engine's internal resolution matches that of the 3DS's 
// screen, they probably wouldn't look too great if implemented anyways
void RenderDevice::LoadShader(const char* fileName, bool32 linear)
{
  return;
}

bool RenderDevice::InitShaders()
{
  return true;
}

bool RenderDevice::SetupRendering()
{
  // is anything even really needed here?
  return true;
}

void RenderDevice::InitVertexBuffer()
{
  // TODO: is this needed, since the buffer is copied to the screen in SW?
  return;
}

bool RenderDevice::InitGraphicsAPI()
{
  // TODO: implement, if needed 
  return true;
}

void RenderDevice::GetDisplays()
{
  // TODO: implement, if needed
  return;
}

void RenderDevice::GetWindowSize(int32* width, int32* height) {
  if (width)
    *width = 480;

  if (height)
    *height = 272;
}
