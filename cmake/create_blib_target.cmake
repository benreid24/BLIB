function(create_blib_target)
	include(configure_blib_target)
    include(link_blib_target)

	cmake_parse_arguments(
        PARSE_ARGV
        0
        ARG
        ""
        "TARGET;OUTPUT_DIR;OUTPUT_FILE"
        "SOURCES;LINK_LIBRARIES"
    )

    if(NOT DEFINED ARG_TARGET)
        message(FATAL_ERROR "create_blib_target requires TARGET_NAME")
    endif()
    if(NOT DEFINED ARG_OUTPUT_DIR)
        set(ARG_OUTPUT_DIR "${CMAKE_SOURCE_DIR}")
    endif()
    if(NOT DEFINED ARG_OUTPUT_FILE)
        set(ARG_OUTPUT_FILE "${ARG_TARGET}${CMAKE_BUILD_TYPE}")
    endif()

    add_executable(${ARG_TARGET} "${BLIB_PATH}/src/main.cpp" "${ARG_SOURCES}")
    set_target_properties(
        ${ARG_TARGET}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${ARG_OUTPUT_DIR}"
        OUTPUT_NAME "${ARG_OUTPUT_FILE}"
    )

    configure_blib_target("${ARG_TARGET}")
    link_blib_target("${ARG_TARGET}" "${ARG_LINK_LIBRARIES}")
endfunction()
