include(compile_shaders)
compile_shaders(
    TARGET BLIB
    SHADER_MODULE blib_builtin_shaders
    
    SHADERS
	empty.vert

    3D/mesh.vert
    3D/meshLit.frag
    3D/meshUnlit.frag
    
    3D/meshMaterial.vert
    3D/meshMaterialLit.frag
    3D/meshMaterialUnlit.frag

    3D/meshSkinned.vert

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
