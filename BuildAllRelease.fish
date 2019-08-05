#!/bin/fish

if not contains -- --no-win $argv; and not contains -- --only-web $argv; and not contains -- --only-linux $argv
	./InitDependencies.fish
else
	./InitDependencies.fish --no-win
end

if not contains -- --no-linux $argv; and not contains -- --only-web $argv; and not contains -- --only-win $argv
	mkdir -p .build/Release-Linux
	cmake -DCMAKE_BUILD_TYPE=Release -H. -B.build/Release-Linux
	make -j4 -C .build/Release-Linux
end

if not contains -- --no-win $argv; and not contains -- --only-web $argv; and not contains -- --only-linux $argv
	mkdir -p .build/Release-Win32
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=(realpath ./CMake/MinGWToolchain.cmake) -H. -B.build/Release-Win32
	make -j4 -C .build/Release-Win32
end

if not contains -- --no-web $argv; and not contains -- --only-win $argv; and not contains -- --only-linux $argv
	mkdir -p .build/Release-Web
	emcmake cmake -DCMAKE_BUILD_TYPE=Release -H. -B.build/Release-Web
	emmake make -j4 -C .build/Release-Web
end
