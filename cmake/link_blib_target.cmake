function(link_blib_target target_name)
	target_link_libraries(${target_name}
		volk
		BLIB
		${ARGN}
		sfml-graphics
		sfml-window
		sfml-network
		sfml-audio
		sfml-system
	)
endfunction()
