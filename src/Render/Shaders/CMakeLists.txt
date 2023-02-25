include(compile_shaders)
compile_shaders(
    TARGET BLIB
    SHADER_MODULE blib_builtin_shaders
    
    SHADERS
	empty.vert
    imageOverlay.frag
    test.frag
    test.vert
    testSkinned.frag
)
