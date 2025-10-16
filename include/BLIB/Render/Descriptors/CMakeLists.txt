target_sources(BLIB PUBLIC
	DescriptorSetFactory.hpp
	DescriptorSetFactoryCache.hpp
	DescriptorSetInstance.hpp
	DescriptorSetInstanceCache.hpp
	GenericDescriptorSetFactory.hpp
	GenericDescriptorSetInstance.hpp
	InstanceTable.hpp
	SetWriteHelper.hpp
)

add_subdirectory(Builtin)
add_subdirectory(Generic)
