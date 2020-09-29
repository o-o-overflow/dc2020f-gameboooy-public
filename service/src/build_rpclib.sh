#!/bin/bash
git clone https://github.com/rpclib/rpclib.git
cd rpclib
mkdir build
cd build
cmake ..
make
