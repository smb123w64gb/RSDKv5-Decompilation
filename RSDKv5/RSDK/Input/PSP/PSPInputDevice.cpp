using namespace RSDK;
#include <pspctrl.h>
SceCtrlLatch latchData;

InputState mappings[12];

static int32 remap[12] = {
  PSP_CTRL_UP,
  PSP_CTRL_DOWN,
  PSP_CTRL_LEFT,
  PSP_CTRL_RIGHT,
  PSP_CTRL_CROSS,
  PSP_CTRL_CIRCLE,
  NULL,
  PSP_CTRL_SQUARE,
  PSP_CTRL_TRIANGLE,
  NULL,
  PSP_CTRL_START,
  PSP_CTRL_SELECT
};

void RSDK::SKU::InputDevicePSP::UpdateInput()
{
  sceCtrlReadLatch(&latchData);

  int32 kDown = latchData.uiMake;
  int32 kHeld = latchData.uiPress;

  if (kDown || kHeld)
    this->anyPress = 1;

  for (int i = 0; i < 12; i++) {
    mappings[i].down = kDown & remap[i];
    mappings[i].press = kHeld & remap[i];
  }
}

// TODO: the code below *technically* works, but is kind of a mess.
// Clean up later.
void RSDK::SKU::InputDevicePSP::ProcessInput(int32 controllerID)
{
  for (int i = 0; i < PLAYER_COUNT; i++) {
    if (i == 2)
      continue;

    controller[i].keyUp.press       |= mappings[0].press;
    controller[i].keyDown.press     |= mappings[1].press;
    controller[i].keyLeft.press     |= mappings[2].press;
    controller[i].keyRight.press    |= mappings[3].press;
    controller[i].keyA.press        |= mappings[4].press;
    controller[i].keyB.press        |= mappings[5].press;
    controller[i].keyC.press        |= mappings[6].press;
    controller[i].keyX.press        |= mappings[7].press;
    controller[i].keyY.press        |= mappings[8].press;
    controller[i].keyZ.press        |= mappings[9].press;
    controller[i].keyStart.press    |= mappings[10].press;
    controller[i].keySelect.press   |= mappings[11].press;
  }
}

// code below here borrowed liberally from the other backends and
// modified accordingly
RSDK::SKU::InputDevicePSP *RSDK::SKU::InitPSPDevice(uint32 id) {
  if (inputDeviceCount == INPUTDEVICE_COUNT)
    return NULL;

  if (inputDeviceList[inputDeviceCount] && 
      inputDeviceList[inputDeviceCount]->active)
    return NULL;

  if (inputDeviceList[inputDeviceCount])
    delete inputDeviceList[inputDeviceCount];

  inputDeviceList[inputDeviceCount] = new InputDevicePSP();

  InputDevicePSP *device = (InputDevicePSP*) inputDeviceList[inputDeviceCount];
  device->gamepadType = (DEVICE_API_NONE << 16) | (DEVICE_TYPE_CONTROLLER << 8) | (DEVICE_SWITCH_HANDHELD << 0);
  device->disabled = false;
  device->id = id;
  device->active = true;
  device->anyPress = 1;

  inputSlots[0] = device->id;
  inputSlotDevices[0] = device;
  device->isAssigned = true;
  
  inputDeviceCount++;
  return device;
}

void RSDK::SKU::InitPSPInputAPI() {
  uint32 id = 1;
  GenerateHashCRC(&id, "PSPDevice0");

  inputDeviceCount = 0;

  InputDevicePSP* device = InitPSPDevice(id);
  if (device) {
    device->controllerID = 1;
  }

  return;
}
