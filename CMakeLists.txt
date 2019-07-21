cmake_minimum_required(VERSION 3.13)
project(JamLib)

file(GLOB_RECURSE SOURCE_FILES Src/Lib/*.cpp Src/Lib/*.hpp)

add_compile_options(-Wall -Wextra -Wshadow -pedantic -Wno-unused-parameter -Wno-missing-field-initializers)

add_library(jam SHARED ${SOURCE_FILES} Deps/opengl/src/gl3w.c)
add_executable(sandbox Src/Sandbox/Main.cpp)

add_library(SDL2 STATIC IMPORTED)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Deps/sdl2/build/linux/libSDL2.a)
	target_link_libraries(jam PRIVATE SDL2)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Deps/sdl2/build/mingw/libSDL2.a)
	target_link_libraries(SDL2 INTERFACE imm32 version winmm)
	target_link_libraries(jam PRIVATE SDL2)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	target_link_options(SDL2 INTERFACE "-s USE_SDL=2")
endif()

set(OUT_DIR ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME})

target_link_libraries(sandbox PRIVATE jam)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	find_package(Threads REQUIRED)
	target_link_libraries(jam PUBLIC Threads::Threads dl)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	set_target_properties(jam sandbox PROPERTIES
		LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s WASM=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s FETCH=1"
	)
	target_compile_options(jam PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
endif()

set_target_properties(jam sandbox PROPERTIES
	CXX_STANDARD 17
	ARCHIVE_OUTPUT_DIRECTORY ${OUT_DIR}
	LIBRARY_OUTPUT_DIRECTORY ${OUT_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}
)

set_target_properties(jam PROPERTIES
	WINDOWS_EXPORT_ALL_SYMBOLS TRUE
	PREFIX ""
	OUTPUT_NAME "jamlib"
	DEFINE_SYMBOL "JM_BUILDING_LIB"
)

target_include_directories(jam
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Src/Lib/>
)

target_include_directories(jam SYSTEM
	PRIVATE
		Deps/opengl/include
		Deps/sdl2/include
		Deps/stb
		Deps/utfcpp/source
		Deps/gsl/include
		Deps/glm
)
