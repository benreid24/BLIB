add_executable(audio main.cpp)

target_link_libraries(audio
    BLIB
    sfml-graphics
    sfml-audio
    sfml-window
)

set(BUILD_SHARED_LIBS Off)
set(SFML_USE_STATIC_STD_LIBS On)
target_compile_definitions(audio PUBLIC SFML_STATIC)
set_target_properties(sfml-audio PROPERTIES CXX_STANDARD 98 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if (WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBRARIES OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

set_target_properties(audio
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
