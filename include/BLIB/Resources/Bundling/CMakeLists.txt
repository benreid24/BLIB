target_sources(BLIB PUBLIC
    Bundle.hpp
    BundledFileMetadata.hpp
    BundleMetadata.hpp
    BundleRuntime.hpp
    BundleSource.hpp
    Config.hpp
    FileHandler.hpp
    FileHandlerContext.hpp
    Manifest.hpp
)

add_subdirectory(FileHandlers)
