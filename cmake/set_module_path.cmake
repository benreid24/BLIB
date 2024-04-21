set(blib_cmake_dir ${CMAKE_CURRENT_LIST_DIR})

function(set_module_path)
    set(
        CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
        "${blib_cmake_dir}"
        "${blib_cmake_dir}/../lib/SFML/cmake"
        "${blib_cmake_dir}/../lib/SFML/cmake/modules"

        PARENT_SCOPE
    )
endfunction()
