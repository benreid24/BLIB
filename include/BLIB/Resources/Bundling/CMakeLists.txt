target_sources(BLIB PUBLIC
    BundledFileMetadata.hpp
    BundleMetadata.hpp
    Config.hpp
    BundleSource.hpp
    FileHandler.hpp
    FileHandlerContext.hpp
    Manifest.hpp
)

add_subdirectory(FileHandlers)
