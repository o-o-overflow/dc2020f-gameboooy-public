#!/bin/bash

#./build_rpclib.sh

cd ./cpu
make
cd ..
cd ./input
make
cd ..
cd ./gpu
make
cd ..
cd ./romloader
make
cd ..
cd ./sound
make
cd ..
cd ./bus
make
cd ..
