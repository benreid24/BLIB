target_sources(BLIB PRIVATE
	Asset.cpp
	AssetFactory.cpp
	AssetPool.cpp
	GraphAssetPool.cpp
	RenderGraph.cpp
	TaskAssets.cpp
)

add_subdirectory(Assets)
add_subdirectory(Strategies)
