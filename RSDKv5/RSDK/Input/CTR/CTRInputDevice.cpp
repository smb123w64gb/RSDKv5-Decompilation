using namespace RSDK;

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

  for (int i = 0; i < 12; i++) {
    this->mappings[i]->down = kDown & remap[i];
    this->mappings[i]->press = kHeld & remap[i];
  }
}

void RSDK::SKU::InputDeviceCTR::ProcessInput(int32 controllerID)
{
  if (controllerID != 0)
    return;

  controller[controllerID].keyUp       = *this->mappings[0];
  controller[controllerID].keyDown     = *this->mappings[1];
  controller[controllerID].keyLeft     = *this->mappings[2];
  controller[controllerID].keyRight    = *this->mappings[3];
  controller[controllerID].keyA        = *this->mappings[4];
  controller[controllerID].keyB        = *this->mappings[5];
  controller[controllerID].keyC        = *this->mappings[6];
  controller[controllerID].keyX        = *this->mappings[7];
  controller[controllerID].keyY        = *this->mappings[8];
  controller[controllerID].keyZ        = *this->mappings[9];
  controller[controllerID].keyStart    = *this->mappings[10];
  controller[controllerID].keySelect   = *this->mappings[11];
}

void RSDK::SKU::InitCTRInputAPI() {
  inputDeviceList[0] = new InputDeviceCTR();
  inputDeviceList[0]->isAssigned = true;
  inputSlotDevices[0] = inputDeviceList[0];
  inputDeviceCount++;

  return;
}
