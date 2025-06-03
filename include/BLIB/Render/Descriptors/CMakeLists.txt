target_sources(BLIB PUBLIC
	DescriptorSetFactory.hpp
	DescriptorSetFactoryCache.hpp
	DescriptorSetInstance.hpp
	DescriptorSetInstanceCache.hpp
	EntityComponentShaderInput.hpp
	GenericDescriptorSetFactory.hpp
	GenericDescriptorSetInstance.hpp
	SceneDescriptorSetInstance.hpp
	SetWriteHelper.hpp
	ShaderInput.hpp
	ShaderInputStore.hpp
)

add_subdirectory(Builtin)
add_subdirectory(Generic)
