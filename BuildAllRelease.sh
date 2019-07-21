#!/bin/bash

./InitDependencies.sh

mkdir -p .build/Release-Linux .build/Release-Win32 .build/Release-Web

cmake -DCMAKE_BUILD_TYPE=Release -H. -B.build/Release-Linux
make -j4 -C .build/Release-Linux

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$(realpath ./CMake/MinGWToolchain.cmake) -H. -B.build/Release-Win32
make -j4 -C .build/Release-Win32

emcmake cmake -DCMAKE_BUILD_TYPE=Release -H. -B.build/Release-Web
emmake make -j4 -C .build/Release-Web
