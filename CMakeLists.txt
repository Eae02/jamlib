cmake_minimum_required(VERSION 3.13)
project(JamLib)

file(GLOB_RECURSE SOURCE_FILES Src/*.cpp Src/*.hpp Deps/yaml-cpp/src/*.cpp)

add_compile_options(-Wall -Wextra -Wshadow -pedantic -Wno-unused-parameter -Wno-missing-field-initializers)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	set(JAM_LIB_TYPE STATIC)
else()
	set(JAM_LIB_TYPE SHARED)
endif()

add_library(jam ${JAM_LIB_TYPE} ${SOURCE_FILES} Deps/opengl/src/gl3w.c Deps/miniz/miniz.c Deps/tinyxml/tinyxml2.cpp)
add_executable(sandbox Sandbox/Main.cpp)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_library(SDL2 STATIC IMPORTED)
	set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Deps/sdl2/build/linux/libSDL2.a)
	
	add_library(OpenAL SHARED IMPORTED)
	set_target_properties(OpenAL PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Deps/openal/build/linux/libopenal.so)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_library(SDL2 STATIC IMPORTED)
	set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Deps/sdl2/build/mingw/libSDL2.a)
	target_link_libraries(SDL2 INTERFACE imm32 version winmm)
	
	add_library(OpenAL STATIC IMPORTED)
	set_target_properties(OpenAL PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Deps/openal/build/mingw/libOpenAL32.dll.a)
endif()

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	target_link_libraries(jam PRIVATE SDL2 OpenAL)
endif()

set(BUILD_NAME ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME})
set(OUT_DIR ${CMAKE_SOURCE_DIR}/Bin/${BUILD_NAME})

target_link_libraries(sandbox PRIVATE jam)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	target_link_libraries(jam PUBLIC pthread dl)
	set_target_properties(jam sandbox PROPERTIES
		INSTALL_RPATH "$ORIGIN"
		BUILD_WITH_INSTALL_RPATH TRUE)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_link_options(jam INTERFACE "-Wl,-subsystem,windows")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	set_target_properties(jam sandbox PROPERTIES
		LINK_FLAGS "-lopenal -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s WASM=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s FETCH=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']"
	)
	target_compile_options(jam PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17> $<$<CONFIG:DEBUG>:-g4>)
	set_target_properties(sandbox PROPERTIES
		OUTPUT_NAME "game"
	)
	add_custom_command(POST_BUILD TARGET jam
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/WebIndex.html $<TARGET_FILE_DIR:jam>/index.html
	)
endif()

target_compile_options(jam PUBLIC -DGLM_FORCE_RADIANS -DGLM_FORCE_CTOR_INIT -DGLM_ENABLE_EXPERIMENTAL)

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
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Src/>
)

target_include_directories(jam SYSTEM
	PUBLIC
		Deps/gsl/include
		Deps/glm
		Deps/pcg/include
	PRIVATE
		Deps/yaml-cpp/include
		Deps/tinyxml
		Deps/miniz
		Deps/openal/include
		Deps/opengl/include
		Deps/sdl2/include
		Deps/stb
		Deps/utfcpp/source
)

export(TARGETS jam FILE ${CMAKE_CURRENT_SOURCE_DIR}/CMake/${BUILD_NAME}/JamTargets.cmake)
