function(configure_target target_name)
    if (APPLE)
        target_compile_definitions(${target_name} PUBLIC BLIB_APPLE=1)
    elseif(WIN32)
        target_compile_definitions(${target_name} PUBLIC BLIB_WINDOWS=1)
    else()
        target_compile_definitions(${target_name} PUBLIC BLIB_LINUX=1) # we don't really care about Sun/IBM/etc
    endif()

    target_compile_definitions(${target_name} PUBLIC GLAD_VULKAN_IMPLEMENTATION)

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

    # Include directories
    target_include_directories(${target_name} PUBLIC 
        "${BLIB_CMAKE_PATH}/include"
        "${BLIB_CMAKE_PATH}/lib/SFML/include"
        "${BLIB_CMAKE_PATH}/lib/glm"
    )
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${target_name} PUBLIC BLIB_DEBUG)
        target_include_directories(${target_name} PUBLIC "lib/glad/glad_debug/include")
    else()
        target_compile_definitions(${target_name} PUBLIC BLIB_RELEASE)
        target_include_directories(${target_name} PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/glad_release/include")
    endif()

    # Static link everything
    target_compile_definitions(${target_name} PUBLIC SFML_STATIC)
    set(BUILD_SHARED_LIBRARIES OFF)
    if (NOT APPLE AND NOT WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif()
endfunction()
