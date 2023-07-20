target_sources(BLIB PUBLIC
	Asset.hpp
	AssetFactory.hpp
	AssetPool.hpp
	AssetProvider.hpp
	AssetRef.hpp
	AssetTags.hpp
	ExecutionContext.hpp
	GraphAsset.hpp
	GraphAssetPool.hpp
	RenderGraph.hpp
	Task.hpp
	TaskAssets.hpp
	TaskAssetTags.hpp
	TaskInput.hpp
)

add_subdirectory(Assets)
add_subdirectory(Strategies)
add_subdirectory(Tasks)
