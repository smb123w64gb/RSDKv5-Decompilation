#!/bin/bash

if [ -z "$1" ]; then
    echo "Missing path to elf file."
    echo "Usage: $0 path/to/file.elf"
    exit 1
fi

APP_NAME="Sonic Mania"
APP_AUTH="SaturnSH2x2, Rubberduckycooly"

mkdir -p out
rm -rf out/*

# make banner
./bannertool makebanner -i ./banner.png -a ./jingle.wav -o out/banner.bnr

# make SMDH
./bannertool makesmdh -s "$APP_NAME" -l "$APP_NAME" -p "$APP_AUTH" -i ./48x48.png -o out/icon.icn

# build CIA
./makerom -f cia -o out/SonicMania.cia -DAPP_ENCRYPTED=false -elf "$1" -rsf ./SonicMania.rsf -exefslogo -target t -icon out/icon.icn -banner out/banner.bnr

echo "Built .cia"
