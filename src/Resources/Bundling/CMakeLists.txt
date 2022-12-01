target_sources(BLIB PRIVATE
    Bundle.cpp
    BundleMetadata.cpp
    BundleRuntime.cpp
    Config.cpp
    FileHandlerContext.cpp
    Manifest.cpp
)

add_subdirectory(FileHandlers)
