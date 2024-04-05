include(compile_shaders)
compile_shaders(
    TARGET particles
    SHADER_MODULE particle_shaders
    
    SHADERS
	particle.vert
)