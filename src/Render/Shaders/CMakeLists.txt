include(compile_shaders)
compile_shaders(
    TARGET BLIB
    SHADER_MODULE blib_builtin_shaders
    
    SHADERS
	empty.vert

    3D/mesh.frag
    3D/mesh.vert
    3D/skinnedMesh.vert
    3D/skinnedMesh.frag

    2D/skinned2d.vert
    2D/skinned2d.frag
    2D/skinned2dlit.frag
    2D/rotatedParticle.frag

    2D/2d.vert
    2D/2d.frag
    2D/2dlit.frag
    
    2D/text.frag
    2D/slideshow.vert

    PostFX/fadeEffect.frag
)
