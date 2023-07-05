target_sources(BLIB PUBLIC
	DescriptorComponentStorage.hpp
	DescriptorComponentStorageCache.hpp
	DescriptorSetFactory.hpp
	DescriptorSetFactoryCache.hpp
	DescriptorSetInstance.hpp
	DescriptorSetInstanceCache.hpp
	SceneDescriptorSetInstance.hpp
)

add_subdirectory(Builtin)
