include(compile_shaders)
compile_shaders(
    TARGET BLIB
    SHADER_MODULE blib_builtin_shaders
    
    SHADERS
	empty.vert

    3D/mesh.vert
    3D/Forward/mesh.frag
    
    3D/meshMaterial.vert
    3D/Forward/meshMaterial.frag

    3D/meshSkinned.vert

    3D/skybox.vert
    3D/skybox.frag

    3D/outline.vert
    3D/outlineSkinned.vert
    3D/outline.frag

    3D/Shadows/shadowMap.vert
    3D/Shadows/pointShadowMap.vert
    3D/Shadows/pointShadowMap.geom
    3D/Shadows/pointShadowMap.frag

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
    PostFX/3dPostProcess.frag
    PostFX/bloomBlur.frag
    PostFX/bloomHighlightFilter.frag
)
