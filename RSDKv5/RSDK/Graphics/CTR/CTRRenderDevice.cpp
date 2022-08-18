// compiler throws an error unless this is included?
#include <3ds.h>

bool RenderDevice::Init()
{
  gfxInit(GSP_RGB565_OES, GSP_RGB565_OES, true);
  consoleInit(GFX_BOTTOM, NULL);

  // dummy out later maybe possibly
  printf("RSDKv5 3DS: RenderDevice init\n \
          The programmer has a nap. Hold out! Programmer!\n");

  return true;
}

void RenderDevice::CopyFrameBuffer()
{
  // TODO: implement
  return;
}

void RenderDevice::ProcessDimming()
{
  // TODO: figure out what this is, THEN implement
  return;
}

void RenderDevice::FlipScreen()
{
  gfxFlushBuffers();
}

void RenderDevice::Release(bool32 isRefresh)
{
  if (!isRefresh) {
    gfxExit();
  }
}

void RenderDevice::RefreshWindow()
{
  gfxSwapBuffers();
  gspWaitForVBlank();
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
  return aptMainLoop();
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
  *width = 400;
  *height = 240;
}
