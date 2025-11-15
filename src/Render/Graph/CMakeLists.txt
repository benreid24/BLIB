target_sources(BLIB PRIVATE
	Asset.cpp
	AssetFactory.cpp
	AssetPool.cpp
	InitContext.cpp
	GraphAssetPool.cpp
	RenderGraph.cpp
	Task.cpp
	TaskAssets.cpp
	Timeline.cpp
)

add_subdirectory(Assets)
add_subdirectory(Providers)
add_subdirectory(Strategies)
add_subdirectory(Tasks)
