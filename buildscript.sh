#!/bin/bash
set -ex

echo "## Cloning repos"
git clone --recursive https://github.com/Rubberduckycooly/Sonic-Mania-Decompilation.git
git clone --recursive https://github.com/SaturnSH2x2/RSDKv5-Decompilation.git Sonic-Mania-Decompilation/dependencies/v5_3DS

echo "## Switching to 3ds-main branch"
cd Sonic-Mania-Decompilation/dependencies/v5_3DS
git checkout 3ds-main
cd ../../

echo "## Creating build directory"
mkdir build
cd build

echo "## Building"
cmake .. -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/3DS.cmake -DRSDK_PATH="dependencies/v5_3DS" -DGAME_STATIC=ON
make -j$(nproc)

echo "## Done, build is in $(pwd)/dependencies/RSDKv5/RetroEngine.3dsx"
