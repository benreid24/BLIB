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
    target_compile_definitions(${target_name} PUBLIC SFML_STATIC)
    set(BUILD_SHARED_LIBRARIES OFF)
    if (NOT APPLE AND NOT WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif()
endfunction()
