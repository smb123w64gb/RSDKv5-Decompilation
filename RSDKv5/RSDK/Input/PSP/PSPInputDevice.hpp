namespace SKU 
{
  struct InputDevicePSP : InputDevice {
    void UpdateInput();
    void ProcessInput(int32 controllerID);

    uint16 buttonMasks;
    uint16 prevButtonMasks;
    uint8 controllerID;
    int32 mouseHideTimer;
    uint8 stateUp;
    uint8 stateDown;
    uint8 stateLeft;
    uint8 stateRight;
    uint8 stateA;
    uint8 stateB;
    uint8 stateC;
    uint8 stateX;
    uint8 stateY;
    uint8 stateZ;
    uint8 stateStart;
    uint8 stateSelect;
  };

  void InitPSPInputAPI();
  InputDevicePSP* InitPSPDevice(uint32 id);
}
