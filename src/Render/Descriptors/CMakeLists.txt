target_sources(BLIB PRIVATE
	DescriptorSetFactoryCache.cpp
	DescriptorSetInstance.cpp
	DescriptorSetInstanceCache.cpp
	SceneDescriptorSetInstance.cpp
	SetWriteHelper.cpp
	ShaderInputStore.cpp
)

add_subdirectory(Builtin)
