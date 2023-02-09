#!/bin/bash 

APP_NAME="Sonic Mania"
APP_AUTH="SaturnSH2x2, Rubberduckycooly"

mkdir -p ../bin/3DS/CTR/out &&
rm -rf ../bin/3DS/CTR/out/*
rm -rf ../bin/3DS/CTR/SonicMania.cia

# super user
sudo su

# make banner
bannertool makebanner -i ./banner.png -a ./jingle.wav -o ../bin/3DS/CTR/out/banner.bnr &&

# make SMDH
bannertool makesmdh -s $APP_NAME -l $APP_NAME -p $APP_AUTH -i ./48x48.png -o ../bin/3DS/CTR/out/icon.icn &&

# build CIA
makerom -f cia -o ../bin/3DS/CTR/SonicMania.cia -DAPP_ENCRYPTED=false -elf ../bin/3DS/CTR/RSDKv5.elf -rsf ./SonicMania.rsf -exefslogo -target t -icon ../bin/3DS/CTR/out/icon.icn -banner ../bin/3DS/CTR/out/banner.bnr &&

echo "Built .cia"

rm -rf ../bin/3DS/CTR/out
