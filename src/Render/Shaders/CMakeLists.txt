include(compile_shaders)
compile_shaders(
    TARGET BLIB
    SHADER_MODULE blib_builtin_shaders
    
    SHADERS
	empty.vert
    imageOverlay.frag
    mesh.frag
    mesh.vert
    skinnedMesh.vert
    skinnedMesh.frag
    2d.vert
    2d.frag
    skinned2d.vert
    skinned2d.frag
    2dlit.vert
    2dlit.frag
    skinned2dlit.vert
    skinned2dlit.frag
    text.frag
)
