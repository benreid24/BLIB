function(configure_blib_target target_name)
    # Platform detection
    if (APPLE)
        target_compile_definitions(${target_name} PUBLIC
            BLIB_MACOS=1
            VK_USE_PLATFORM_MACOS_MVK
        )

        set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_MACOS_MVK)
    elseif(WIN32)
        target_compile_definitions(${target_name} PUBLIC
            BLIB_WINDOWS=1
        )

        set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
    else()
        target_compile_definitions(${target_name} PUBLIC
            BLIB_LINUX=1
        )

        set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XLIB_KHR)
    endif()

    # compile defs for Vulkan
    target_compile_definitions(${target_name} PUBLIC
        GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
        GLM_FORCE_DEPTH_ZERO_TO_ONE
    )

    # VMA setup
    target_compile_definitions(${target_name} PUBLIC
        VMA_STATIC_VULKAN_FUNCTIONS=0
        VMA_DYNAMIC_VULKAN_FUNCTIONS=0
    )
    target_include_directories(${target_name} PUBLIC ${BLIB_PATH}/lib/VulkanMemoryAllocator/include)

    # Warnings
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
            /wd26495
        )

        # No warnings from VMA
        set_source_files_properties(
            ${BLIB_PATH}/lib/VulkanMemoryAllocator/include/vk_mem_alloc.h
            PROPERTIES
            COMPILE_OPTIONS /w
        )
    else()
        # lots of warnings
        target_compile_options(${target_name} PUBLIC -Wall -Wextra -pedantic)
        # No warnings from VMA
        set_source_files_properties(
            ${BLIB_PATH}/lib/VulkanMemoryAllocator/include/vk_mem_alloc.h
            PROPERTIES
            COMPILE_OPTIONS -w
        )
        # Disable clang extension warning on MacOS
        if(APPLE)
            target_compile_options(${target_name} PUBLIC
                -Wno-nullability-extension
                -Wno-nullability-completeness
            )
        endif()
    endif()

    # Include directories
    target_include_directories(${target_name} PUBLIC
        ${BLIB_PATH}/include
        ${BLIB_TEMP_INCLUDE_DIR}
    )
    target_include_directories(${target_name} SYSTEM PUBLIC 
        ${BLIB_PATH}/lib/SFML/include
        ${BLIB_PATH}/lib/glm
        ${BLIB_PATH}/lib/Vulkan-Headers/include
        ${BLIB_PATH}/lib/volk
    )
    if(APPLE)
        target_include_directories(${target_name} SYSTEM PUBLIC ${BLIB_PATH}/lib/glfw/include)
    endif()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${target_name} PUBLIC BLIB_DEBUG)
    else()
        target_compile_definitions(${target_name} PUBLIC BLIB_RELEASE)
    endif()
    
    # Static link everything
    target_compile_definitions(${target_name} PUBLIC SFML_STATIC)
    set(BUILD_SHARED_LIBRARIES OFF)
    if (NOT APPLE AND NOT WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif()

    # Expose shader directory
    file(RELATIVE_PATH BLIB_SHADER_PATH ${PROJECT_SOURCE_DIR} "${BLIB_PATH}/src/Render/Shaders") # hopefully relative to top level
    target_compile_definitions(${target_name} PUBLIC BLIB_SHADER_PATH=${BLIB_SHADER_PATH})

    # ECS ComponentMask type
    if(${BLIB_ECS_USE_WIDE_MASK})
        target_compile_definitions(${target_name} PUBLIC BLIB_ECS_MASK_TYPE=std::uint128_t)
    else()
        target_compile_definitions(${target_name} PUBLIC BLIB_ECS_MASK_TYPE=std::uint64_t)
    endif()
endfunction()
