function(link_blib_target target_name)
	set(extra_link_libs "${ARGN}")

	if(APPLE)
		list(APPEND extra_link_libs glfw)
	endif()

	if(NOT "${target_name}" STREQUAL "BLIB")
		list(APPEND extra_link_libs BLIB)
	endif()

	target_link_libraries(${target_name}
		volk
		Freetype
		${extra_link_libs}
		sfml-graphics
		sfml-window
		sfml-network
		sfml-audio
		sfml-system
	)
endfunction()
