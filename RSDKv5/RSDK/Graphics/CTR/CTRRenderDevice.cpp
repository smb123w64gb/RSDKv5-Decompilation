#define SCREEN_XSIZE (400)
#define SCREEN_YSIZE (240)

#define SCREEN_REFRESH (59.94f)

using namespace RSDK;

static float msPerFrame = 1000.0f / SCREEN_REFRESH;

u64 curFrame;
u64 prevFrame;
float msElapsed;

bool RenderDevice::Init()
{
  gfxInitDefault();

  if (printToConsole) {
    consoleInit(GFX_BOTTOM, NULL);

    // dummy out later maybe possibly
    printf("RSDKv5 3DS: RenderDevice init\n");
    printf("The programmer has a nap. Hold out!\nProgrammer!\n");
  }

  gfxSetScreenFormat(GFX_TOP, GSP_RGB565_OES);
  gfxSetDoubleBuffering(GFX_TOP, true);

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
  u16 *src = screens[0].frameBuffer;
  u16 *dst = (uint16*) gfxGetFramebuffer(GFX_TOP, GFX_LEFT, 0, 0);

  if (!CheckForFrameSkip()) {
    switch (videoSettings.screenCount) {
      default:
      case 0:
        // image/video buffer, break
        break;
      case 1:
        for (int y = 0; y < SCREEN_YSIZE; y++) {
          for (int x = 0; x < SCREEN_XSIZE; x++) {
            dst[((x * 240) + (240 - y - 1))] = *src++;
          }
        }
        break;

      // the separate screens used for competition mode will likely 
      // never be supported by the 3DS port
      case 2:
      case 3:
      case 4:
        break;
    }

    gfxFlushBuffers();
    gfxSwapBuffers();
  }

  gspWaitForVBlank();
}

void RenderDevice::Release(bool32 isRefresh)
{
  if (scanlines)
    free(scanlines);

  if (!isRefresh) {
    gfxExit();
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
  return true;
}

void RenderDevice::UpdateFPSCap()
{
  curFrame = osGetTime();
  msElapsed = (float) (curFrame - prevFrame);
  prevFrame = curFrame;
}

bool RenderDevice::CheckForFrameSkip() 
{
  if (msElapsed <= ceil(msPerFrame * 1.2))
    return false;
  else
    return true;    
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
    *width = 400;

  if (height)
    *height = 240;
}
