include("${CMAKE_CURRENT_LIST_DIR}/CMake/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}/JamTargets.cmake")

set(JAM_SO_PATH ${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}/jamlib${CMAKE_SHARED_LIBRARY_SUFFIX})

function(target_link_jamlib GAME_TARGET)
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		set_target_properties(${GAME_TARGET} PROPERTIES
			INSTALL_RPATH "$ORIGIN"
			BUILD_WITH_INSTALL_RPATH TRUE)
	endif()
	
	
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
		set_target_properties(${GAME_TARGET} PROPERTIES
			LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s WASM=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s FETCH=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']"
			OUTPUT_NAME "game"
		)
	else()
		add_custom_command(POST_BUILD TARGET ${GAME_TARGET}
			COMMAND ${CMAKE_COMMAND} -E copy ${JAM_SO_PATH} $<TARGET_FILE_DIR:${GAME_TARGET}>
		)
	endif()
	
	target_link_libraries(${GAME_TARGET} jam)
endfunction()
