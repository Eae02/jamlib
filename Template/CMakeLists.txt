cmake_minimum_required(VERSION 3.13)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/JamLib/Deps/cmake-pch)
project(%PROJECT_NAME% CXX CXXPCH)

include("JamLib/CMakeInclude.cmake")

file(GLOB_RECURSE SOURCE_FILES Src/*.cpp Src/*.hpp WinRes.rc)
add_executable(%PROJECT_NAME% ${SOURCE_FILES})

set_target_properties(%PROJECT_NAME% PROPERTIES
	COMPILE_FLAGS "-Wall -Wextra -Wshadow -pedantic --std=c++17"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}"
)

target_link_jamlib(%PROJECT_NAME%)

target_precompiled_header(%PROJECT_NAME% PCH.hpp)
