function(init_blib_cmake)
    if(MSVC)
        # Use static msvc runtime in sync with assimp
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" PARENT_SCOPE)
    endif()
endfunction()
