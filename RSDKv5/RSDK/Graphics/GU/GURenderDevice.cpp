
#define MANIA_WIDTH (424)//424
#define MANIA_HEIGHT (240)//240

#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
using namespace RSDK;

#define MANIA_PITCH (MANIA_WIDTH + 15) & 0xFFFFFFF0
#define PSP_SCREEN_WIDTH 480
#define PSP_SCREEN_HEIGHT 272
#define PSP_LINE_SIZE 512

#define GE_CMD_FBP    0x9C
#define GE_CMD_FBW    0x9D
#define GE_CMD_TBP0   0xA0
#define GE_CMD_TBW0   0xA8
#define GE_CMD_TSIZE0 0xB8
#define GE_CMD_TFLUSH 0xCB
#define GE_CMD_CLEAR  0xD3
#define GE_CMD_VTYPE  0x12
#define GE_CMD_BASE   0x10
#define GE_CMD_VADDR  0x01
#define GE_CMD_IADDR  0x02
#define GE_CMD_PRIM   0x04
#define GE_CMD_FINISH 0x0F
#define GE_CMD_SIGNAL 0x0C
#define GE_CMD_NOP    0x00

static float *screen_vertex = (float *)0x441FC100;
static u32 *ge_cmd = (u32 *)0x441FC000;
static u16 *psp_gu_vram_base = (u16 *)(0x44000000);
static u32 *ge_cmd_ptr = (u32 *)0x441FC000;
static u32 gecbid;
static u32 video_direct = 0;

static u32 __attribute__((aligned(16))) display_list[32];
static u16 *screen_texture = (u16 *)(0x4000000 + (512 * 272 * 2));
static u16 *current_screen_texture = (u16 *)(0x4000000 + (512 * 272 * 2));
static u16 *screen_pixels = (u16 *)(0x4000000 + (512 * 272 * 2));
static u32 screen_pitch = 424;

#define GE_CMD(cmd, operand)                                                \
  *ge_cmd_ptr = (((GE_CMD_##cmd) << 24) | (operand));                       \
  ge_cmd_ptr++                                                              \

static void Ge_Finish_Callback(int id, void *arg)
{
}

#define get_screen_pixels()                                                 \
  screen_pixels                                                             \

#define get_screen_pitch()                                                  \
  screen_pitch                                                              \

bool RenderDevice::Init()
{//This is just gpSP display code atm...
printf("Mania Pitch is %i",MANIA_PITCH);
  sceDisplaySetMode(0, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT);
  sceDisplayWaitVblankStart();
  sceDisplaySetFrameBuf((void*)psp_gu_vram_base, PSP_LINE_SIZE,
  PSP_DISPLAY_PIXEL_FORMAT_565, PSP_DISPLAY_SETBUF_NEXTFRAME);

  sceGuInit();

  sceGuStart(GU_DIRECT, display_list);
  sceGuDrawBuffer(GU_PSM_5650, (void*)0, PSP_LINE_SIZE);
  sceGuDispBuffer(PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT,
   (void*)0, PSP_LINE_SIZE);
  sceGuClear(GU_COLOR_BUFFER_BIT);

  sceGuOffset(2048 - (PSP_SCREEN_WIDTH / 2), 2048 - (PSP_SCREEN_HEIGHT / 2));
  sceGuViewport(2048, 2048, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT);

  sceGuScissor(0, 0, PSP_SCREEN_WIDTH + 1, PSP_SCREEN_HEIGHT + 1);
  sceGuEnable(GU_SCISSOR_TEST);
  sceGuTexMode(GU_PSM_5650, 0, 0, GU_FALSE);
  sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
  sceGuTexFilter(GU_LINEAR, GU_LINEAR);
  sceGuEnable(GU_TEXTURE_2D);

  sceGuFrontFace(GU_CW);
  sceGuDisable(GU_BLEND);

  sceGuFinish();
  sceGuSync(0, 0);

  sceDisplayWaitVblankStart();
  sceGuDisplay(GU_TRUE);

  PspGeCallbackData gecb;
  gecb.signal_func = NULL;
  gecb.signal_arg = NULL;
  gecb.finish_func = Ge_Finish_Callback;
  gecb.finish_arg = NULL;
  gecbid = sceGeSetCallback(&gecb);

  screen_vertex[0] = 0 + 0.5;
  screen_vertex[1] = 0 + 0.5;
  screen_vertex[2] = 0 + 0.5;
  screen_vertex[3] = 0 + 0.5;
  screen_vertex[4] = 0;
  screen_vertex[5] = MANIA_WIDTH - 0.5;
  screen_vertex[6] = MANIA_HEIGHT - 0.5;
  screen_vertex[7] = PSP_SCREEN_WIDTH - 0.5;
  screen_vertex[8] = PSP_SCREEN_HEIGHT - 0.5;
  screen_vertex[9] = 0;

  // Set framebuffer to PSP VRAM
  GE_CMD(FBP, ((u32)psp_gu_vram_base & 0x00FFFFFF));
  GE_CMD(FBW, (((u32)psp_gu_vram_base & 0xFF000000) >> 8) | PSP_LINE_SIZE);
  // Set texture 0 to the screen texture
  GE_CMD(TBP0, ((u32)screen_texture & 0x00FFFFFF));
  GE_CMD(TBW0, (((u32)screen_texture & 0xFF000000) >> 8) | MANIA_PITCH);
  // Set the texture size to 256 by 256 (2^8 by 2^8)
  GE_CMD(TSIZE0, (8 << 8) | 9);
  // Flush the texture cache
  GE_CMD(TFLUSH, 0);
  // Use 2D coordinates, no indeces, no weights, 32bit float positions,
  // 32bit float texture coordinates
  GE_CMD(VTYPE, (1 << 23) | (0 << 11) | (0 << 9) |
   (3 << 7) | (0 << 5) | (0 << 2) | 3);
  // Set the base of the index list pointer to 0
  GE_CMD(BASE, 0);
  // Set the rest of index list pointer to 0 (not being used)
  GE_CMD(IADDR, 0);
  // Set the base of the screen vertex list pointer
  GE_CMD(BASE, ((u32)screen_vertex & 0xFF000000) >> 8);
  // Set the rest of the screen vertex list pointer
  GE_CMD(VADDR, ((u32)screen_vertex & 0x00FFFFFF));
  // Primitive kick: render sprite (primitive 6), 2 vertices
  GE_CMD(PRIM, (6 << 16) | 2);
  // Done with commands
  GE_CMD(FINISH, 0);
  // Raise signal interrupt
  GE_CMD(SIGNAL, 0);
  GE_CMD(NOP, 0);
  GE_CMD(NOP, 0);

  // dummy out later maybe possibly
  printf("RSDKv5 PSP: RenderDevice init\n");
  printf("The programmer has a nap. Hold out!\nProgrammer!\n");


  scanlines = (ScanlineInfo*) malloc(MANIA_WIDTH * sizeof(ScanlineInfo));
  if (!scanlines)
  return false;

  engine.inFocus = 1;
  videoSettings.windowState = WINDOWSTATE_ACTIVE;
  videoSettings.dimMax = 1.0;
  videoSettings.dimPercent = 1.0;

  RSDK::SetScreenSize(0, MANIA_WIDTH,MANIA_HEIGHT );
  screens[0].frameBuffer = screen_pixels;
  memset(screens[0].frameBuffer, 0, MANIA_HEIGHT * MANIA_PITCH * sizeof(uint16));

  InitInputDevices();
  if (!AudioDevice::Init())
  return false;

  return true;
}

void clear_screen(u16 color)
{
  u32 i;
  u16 *src_ptr = get_screen_pixels();

  sceGuSync(0, 0);

  for(i = 0; i < (MANIA_PITCH * MANIA_HEIGHT); i++, src_ptr++)
  {
    *src_ptr = color;
  }

}

void RenderDevice::CopyFrameBuffer()
{
  return;
}

void RGBtoBGR()
{
        int32 cnt = (MANIA_WIDTH+16) * MANIA_HEIGHT;
        for (int32 id = 0; cnt > 0; --cnt, ++id) {
            uint16 px = screens[0].frameBuffer[id];
            screens[0].frameBuffer[id] = ((px & 0x1F)<< 11) | (px & 0x7E0) | ((px & 0xF800) >> 11);
        }
}

// https://github.com/JeffRuLz/Sonic-1-2-2013-Decompilation/blob/main/RSDKv4/3ds/3ds.cpp#L141
void RenderDevice::FlipScreen()
{
    //RGBtoBGR();
    u32 *old_ge_cmd_ptr = ge_cmd_ptr;
    sceKernelDcacheWritebackAll();
    // Render the current screen
    ge_cmd_ptr = ge_cmd + 2;
    GE_CMD(TBP0, ((u32)screen_pixels & 0x00FFFFFF));
    GE_CMD(TBW0, (((u32)screen_pixels & 0xFF000000) >> 8) |
     MANIA_PITCH);
    ge_cmd_ptr = old_ge_cmd_ptr;
    sceGeListEnQueue(ge_cmd, ge_cmd_ptr, gecbid, NULL);

  switch (videoSettings.screenCount) {
    default:
    case 0:
      // image/video buffer, break
      break;
    case 1:
    //memcpy(screen_pixels,screens[0].frameBuffer,MANIA_HEIGHT * (MANIA_WIDTH+16) * sizeof(uint16));
      //screen_pixels = screens[0].frameBuffer;
      
      
      /*for (int y = 0; y < MANIA_WIDTH; y++) {
          for (int x = 0; x < MANIA_HEIGHT; x++) {
            dst[((x * 512) + (512 - y - 1))] = *src++;
          }
        }*/
    //sceGuCopyImage(GU_PSM_5650,0,0,480,272,512,screens[0].frameBuffer,0,0,512,(void*)(((unsigned int)framebuffer)+0x4000000));
      break;

    // the separate screens used for competition mode will likely 
    // never be supported by the 3DS port
    case 2:
    case 3:
    case 4:
      break;
  }
  //sceDisplayWaitVblankStart();
    //sceGuFinish();
    //sceGuSync(0, 0);
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
