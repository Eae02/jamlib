#!/bin/bash

mkdir -p Deps/opengl
python2 Deps/gl3w/gl3w_gen.py --root Deps/opengl > /dev/null

#Downloads and extracts miniz
if [[ -d "Deps/miniz" ]]; then
	echo "Skipping download of miniz since Deps/miniz already exists"
else
	mkdir -p Deps/miniz
	curl -L https://github.com/richgel999/miniz/releases/download/2.1.0/miniz-2.1.0.zip > Deps/miniz/miniz.zip
	unzip -j Deps/miniz/miniz.zip miniz.c -d Deps/miniz
	unzip -j Deps/miniz/miniz.zip miniz.h -d Deps/miniz
fi

#Downloads and extracts tinyxml
if [[ -d "Deps/tinyxml" ]]; then
	echo "Skipping download of tinyxml since Deps/tinyxml already exists"
else
	mkdir -p Deps/tinyxml
	curl -L https://github.com/leethomason/tinyxml2/archive/7.0.1.zip > Deps/tinyxml/tinyxml.zip
	unzip -j Deps/tinyxml/tinyxml.zip */tinyxml2.cpp -d Deps/tinyxml
	unzip -j Deps/tinyxml/tinyxml.zip */tinyxml2.h -d Deps/tinyxml
fi

#Downloads and extracts SDL2
if [[ -d "Deps/sdl2" ]]; then
	echo "Skipping download of sdl2 since Deps/sdl2 already exists"
else
	mkdir -p Deps/sdl2
	curl https://www.libsdl.org/release/SDL2-2.0.9.tar.gz -sS | tar -C Deps/sdl2 -xzf - --strip 1
fi

#Downloads and extracts OpenAL
if [[ -d "Deps/openal" ]]; then
	echo "Skipping download of openal since Deps/openal already exists"
else
	mkdir -p Deps/openal
	curl https://kcat.strangesoft.net/openal-releases/openal-soft-1.19.1.tar.bz2 -sS | tar -C Deps/openal -xjf - --strip 1
fi

#Builds SDL2 for linux
mkdir -p Deps/sdl2/build/linux
cmake -DCMAKE_BUILD_TYPE=Release -DSNDIO=OFF -DSDL_STATIC_PIC=ON -DSDL_SHARED=OFF -DSDL_STATIC=ON -HDeps/sdl2 -BDeps/sdl2/build/linux
make -j4 -C Deps/sdl2/build/linux

#Builds SDL2 for mingw
mkdir -p Deps/sdl2/build/mingw
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$(realpath ./CMake/MinGWToolchain.cmake) -DSDL_SHARED=OFF -DSDL_STATIC=ON -HDeps/sdl2 -BDeps/sdl2/build/mingw
make -j4 -C Deps/sdl2/build/mingw

#Builds OpenAL for linux
mkdir -p Deps/openal/build/linux
cmake -DCMAKE_BUILD_TYPE=Release -DALSOFT_UTILS=OFF -DALSOFT_NO_CONFIG_UTIL=ON -DALSOFT_EXAMPLES=OFF -DALSOFT_TESTS=OFF -HDeps/openal -BDeps/openal/build/linux
make -j4 -C Deps/openal/build/linux

#Builds OpenAL for mingw
mkdir -p Deps/openal/build/mingw
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$(realpath ./CMake/MinGWToolchain.cmake) -DALSOFT_UTILS=OFF -DALSOFT_NO_CONFIG_UTIL=ON -DALSOFT_EXAMPLES=OFF -DALSOFT_TESTS=OFF -HDeps/openal -BDeps/openal/build/mingw
make -j4 -C Deps/openal/build/mingw
