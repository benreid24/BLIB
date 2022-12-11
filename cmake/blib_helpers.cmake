function(configure_target target_name)
    if (APPLE)
        target_compile_definitions(${target_name} PUBLIC BLIB_APPLE=1)
    elseif(WIN32)
        target_compile_definitions(${target_name} PUBLIC BLIB_WINDOWS=1)
    else()
        target_compile_definitions(${target_name} PUBLIC BLIB_LINUX=1) # we don't really care about Sun/IBM/etc
    endif()

    if (MSVC)
        # warning level 4
        target_compile_options(${target_name} PUBLIC /W4)

        # disable overly pedantic warnings
        target_compile_options(${target_name} PUBLIC
            /wd4244
            /wd4267
            /wd4458
            /wd4456
            /wd4457
            /wd4706
        )
    else()
        # lots of warnings
        target_compile_options(${target_name} PUBLIC -Wall -Wextra -pedantic)
    endif()

    # Static link everything
    set(SFML_USE_STATIC_STD_LIBS On)
    target_compile_definitions(BLIB PUBLIC SFML_STATIC)
    target_compile_definitions(${target_name} PUBLIC SFML_STATIC)
    set(BUILD_SHARED_LIBRARIES OFF)
    set_target_properties(${target_name} PROPERTIES
        LINK_SEARCH_START_STATIC ON
        LINK_SEARCH_END_STATIC ON
    )
    if (CMAKE_COMPILER_IS_GNUXX OR CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        target_link_options(${target_name} PUBLIC -static -static-libgcc -static-libstdc++)
        set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
    endif()
endfunction()
