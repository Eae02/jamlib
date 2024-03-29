set(JAM_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

include("${JAM_ROOT_DIR}/CMake/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}/JamTargets.cmake")

function(target_link_jamlib GAME_TARGET)
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		set_target_properties(${GAME_TARGET} PROPERTIES
			INSTALL_RPATH "$ORIGIN"
			BUILD_WITH_INSTALL_RPATH TRUE)
	endif()
	
	if (EXISTS ${CMAKE_SOURCE_DIR}/.build/icon)
		if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
			add_custom_command(POST_BUILD TARGET ${GAME_TARGET}
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/.build/icon/jm_game_icon $<TARGET_FILE_DIR:${GAME_TARGET}>/favicon.png
			)
		elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
			target_sources(${GAME_TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/.build/icon/icon.o)
			set_source_files_properties(${CMAKE_SOURCE_DIR}/.build/icon/icon.o PROPERTIES
				EXTERNAL_OBJECT true
				GENERATED true
			)
		elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
			target_sources(${GAME_TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/.build/icon/icon.rc)
		endif()
	endif()
	
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
		set_target_properties(${GAME_TARGET} PROPERTIES
			LINK_FLAGS "-lopenal -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s WASM=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s FETCH=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']"
			OUTPUT_NAME "game"
		)
		
		add_custom_command(POST_BUILD TARGET ${GAME_TARGET}
			COMMAND ${CMAKE_COMMAND} -E copy ${JAM_ROOT_DIR}/WebIndex.html $<TARGET_FILE_DIR:${GAME_TARGET}>/index.html
		)
	else()
		if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
			set(OPENAL_PATH ${JAM_ROOT_DIR}/Deps/openal/build/linux/libopenal.so.1)
			target_link_options(${GAME_TARGET} PRIVATE -rdynamic)
		else()
			set(OPENAL_PATH ${JAM_ROOT_DIR}/Deps/openal/build/mingw/OpenAL32.dll)
		endif()
		
		add_custom_command(POST_BUILD TARGET ${GAME_TARGET}
			COMMAND ${CMAKE_COMMAND} -E copy ${JAM_ROOT_DIR}/Bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}/jamlib${CMAKE_SHARED_LIBRARY_SUFFIX} $<TARGET_FILE_DIR:${GAME_TARGET}>
		)
		
		add_custom_command(POST_BUILD TARGET ${GAME_TARGET}
			COMMAND ${CMAKE_COMMAND} -E copy ${OPENAL_PATH} $<TARGET_FILE_DIR:${GAME_TARGET}>
		)
	endif()
	
	target_link_libraries(${GAME_TARGET} jam)
endfunction()
