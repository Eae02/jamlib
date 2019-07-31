#!/bin/bash

mkdir -p .build/Release-Linux .build/Release-Win32 .build/Release-Web

cmake -DCMAKE_BUILD_TYPE=Release -H. -B.build/Release-Linux
make -j4 -C .build/Release-Linux

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="$(dirname "$0")/CMake/MinGWToolchain.cmake" -H. -B.build/Release-Win32
make -j4 -C .build/Release-Win32

emcmake cmake -DCMAKE_BUILD_TYPE=Release -H. -B.build/Release-Web
emmake make -j4 -C .build/Release-Web

zip -FSj Bin/Release-Linux/assets.zip ./Assets/*
cp Bin/Release-Linux/assets.zip Bin/Release-Windows/assets.zip
cp Bin/Release-Linux/assets.zip Bin/Release-Emscripten/assets.zip

cp /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll Bin/Release-Windows
cp /usr/x86_64-w64-mingw32/bin/libstdc++-6.dll Bin/Release-Windows
cp /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll Bin/Release-Windows
