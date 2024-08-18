target_sources(BLIB PUBLIC
	DescriptorComponentStorage.hpp
	DescriptorComponentStorageCache.hpp
	DescriptorSetFactory.hpp
	DescriptorSetFactoryCache.hpp
	DescriptorSetInstance.hpp
	DescriptorSetInstanceCache.hpp
	GenericDescriptorSetFactory.hpp
	GenericDescriptorSetInstance.hpp
	SceneDescriptorSetInstance.hpp
	SetWriteHelper.hpp
)

add_subdirectory(Builtin)
add_subdirectory(Generic)
