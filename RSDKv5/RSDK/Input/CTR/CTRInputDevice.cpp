using namespace RSDK;

InputState mappings[12];

static u32 remap[12] = {
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_Y,
  KEY_B,
  KEY_A,
  KEY_L,
  KEY_X,
  KEY_R,
  KEY_START,
  KEY_SELECT
};

void RSDK::SKU::InputDeviceCTR::UpdateInput()
{
  hidScanInput();

  u32 kDown = hidKeysDown();
  u32 kHeld = hidKeysHeld();

  if (kDown || kHeld)
    this->anyPress = 1;

  for (int i = 0; i < 12; i++) {
    mappings[i].down = kDown & remap[i];
    mappings[i].press = kHeld & remap[i];
  }
}

void RSDK::SKU::InputDeviceCTR::ProcessInput(int32 controllerID)
{
  for (int i = 0; i < 1; i++) {
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
RSDK::SKU::InputDeviceCTR *RSDK::SKU::InitCTRDevice(uint32 id) {
  if (inputDeviceCount == INPUTDEVICE_COUNT)
    return NULL;

  if (inputDeviceList[inputDeviceCount] && 
      inputDeviceList[inputDeviceCount]->active)
    return NULL;

  if (inputDeviceList[inputDeviceCount])
    delete inputDeviceList[inputDeviceCount];

  inputDeviceList[inputDeviceCount] = new InputDeviceCTR();

  InputDeviceCTR *device = (InputDeviceCTR*) inputDeviceList[inputDeviceCount];
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

void RSDK::SKU::InitCTRInputAPI() {
  uint32 id = 0;
  GenerateHashCRC(&id, "CTRDevice0");

  inputDeviceCount = 0;

  InputDeviceCTR* device = InitCTRDevice(id);
  if (device) {
    device->controllerID = 0;
  }

  return;
}
