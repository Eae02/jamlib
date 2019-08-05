#!/bin/fish

mkdir -p .build/Release-Linux .build/Release-Win32 .build/Release-Web

cmake -DCMAKE_BUILD_TYPE=Release -H. -B.build/Release-Linux
make -j4 -C .build/Release-Linux

pushd .
cd ./Assets
zip -FSr ../Bin/Release-Linux/assets.zip .
popd

if test -d "(dirname "$0")/CMake/Release-Windows"
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="(dirname "$0")/CMake/MinGWToolchain.cmake" -H. -B.build/Release-Win32
	make -j4 -C .build/Release-Win32
	
	cp Bin/Release-Linux/assets.zip Bin/Release-Windows/assets.zip
	cp /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll Bin/Release-Windows
	cp /usr/x86_64-w64-mingw32/bin/libstdc++-6.dll Bin/Release-Windows
	cp /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll Bin/Release-Windows
else
	echo "Skipping windows build"
end

if test -d "(dirname "$0")/CMake/Release-Emscripten"
	emcmake cmake -DCMAKE_BUILD_TYPE=Release -H. -B.build/Release-Web
	emmake make -j4 -C .build/Release-Web
	
	cp Bin/Release-Linux/assets.zip Bin/Release-Emscripten/assets.zip
else
	echo "Skipping web build"
end
