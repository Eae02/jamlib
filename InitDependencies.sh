#!/bin/bash

mkdir -p Deps/opengl
python2 Deps/gl3w/gl3w_gen.py --root Deps/opengl > /dev/null

##Downloads and extracts SDL2
if [[ -d "Deps/sdl2" ]]; then
	echo "Skipping download sdl2 since Deps/sdl2 already exists"
else
	mkdir -p Deps/sdl2
	curl https://www.libsdl.org/release/SDL2-2.0.9.tar.gz -sS | tar -C Deps/sdl2 -xzf - --strip 1
fi

#Builds SDL2 for linux
mkdir -p Deps/sdl2/build/linux
cmake -DCMAKE_BUILD_TYPE=Release -DSNDIO=OFF -DSDL_STATIC_PIC=ON -DSDL_SHARED=OFF -DSDL_STATIC=ON -HDeps/sdl2 -BDeps/sdl2/build/linux
make -j4 -C Deps/sdl2/build/linux

#Builds SDL2 for mingw
mkdir -p Deps/sdl2/build/mingw
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$(realpath ./CMake/MinGWToolchain.cmake) -DSDL_SHARED=OFF -DSDL_STATIC=ON -HDeps/sdl2 -BDeps/sdl2/build/mingw
make -j4 -C Deps/sdl2/build/mingw
