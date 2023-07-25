target_sources(BLIB PRIVATE
	Asset.cpp
	AssetFactory.cpp
	AssetPool.cpp
	GraphAssetPool.cpp
	RenderGraph.cpp
	Task.cpp
	TaskAssets.cpp
)

add_subdirectory(Assets)
add_subdirectory(Providers)
add_subdirectory(Strategies)
add_subdirectory(Tasks)
