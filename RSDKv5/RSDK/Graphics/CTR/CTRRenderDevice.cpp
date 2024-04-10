#define SCREEN_XSIZE_3DS (400)
#define SCREEN_YSIZE_3DS (240)

#define SCREEN_REFRESH (59.94f)

using namespace RSDK;

u32 clearColor = 0;
u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
u32 clrCircle1 = C2D_Color32(0xFF, 0x00, 0xFF, 0xFF);
u32 clrCircle2 = C2D_Color32(0xFF, 0xFF, 0x00, 0xFF);
u32 clrCircle3 = C2D_Color32(0x00, 0xFF, 0xFF, 0xFF);

static float msPerFrame = 1000.0f / SCREEN_REFRESH;

u64 curFrame;
u64 prevFrame;
float msElapsed;

bool RenderDevice::Init()
{
  if (printToConsole) {
    consoleInit(GFX_BOTTOM, NULL);
    // dummy out later maybe possibly
    printf("RSDKv5 3DS: RenderDevice init\n");
    printf("The programmer has a nap. Hold out!\nProgrammer!\n");
  }


  scanlines = (ScanlineInfo*) malloc(SCREEN_YSIZE_3DS * sizeof(ScanlineInfo));
  if (!scanlines)
    return false;

  engine.inFocus = 1;
  videoSettings.windowState = WINDOWSTATE_ACTIVE;
  videoSettings.dimMax = 1.0;
  videoSettings.dimPercent = 1.0;

  RSDK::SetScreenSize(0, SCREEN_XSIZE_3DS, SCREEN_YSIZE_3DS);

  memset(screens[0].frameBuffer, 0, SCREEN_XSIZE_3DS * SCREEN_YSIZE_3DS * sizeof(uint16));

  InitInputDevices();
  if (!AudioDevice::Init())
    return false;
  
  return true;
}

void RenderDevice::CopyFrameBuffer()
{
}

// https://github.com/JeffRuLz/Sonic-1-2-2013-Decompilation/blob/main/RSDKv4/3ds/3ds.cpp#L141
void RenderDevice::FlipScreen()
{
}


void RenderDevice::Release(bool32 isRefresh)
{
  if (scanlines)
    free(scanlines);

  if (!isRefresh) {
    C2D_Fini();
	  C3D_Fini();
    gfxExit();
  }
}

void RenderDevice::RefreshWindow()
{
return;
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
  
  C2D_TargetClear(engine.topScreen,engine.clearColor);
  C2D_SceneBegin(engine.topScreen);
  
  

  if (controller->keySelect.press){
  if (engine.devMenu) {
    if (sceneInfo.state == ENGINESTATE_DEVMENU)
        CloseDevMenu();
    else
        OpenDevMenu();
                    }
                    }
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
  if (msElapsed <= ceil(msPerFrame * 2.0))
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
