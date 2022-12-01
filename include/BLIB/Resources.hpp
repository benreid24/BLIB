#ifndef BLIB_RESOURCES_HPP
#define BLIB_RESOURCES_HPP

/**
 * @defgroup Resources
 * @brief A collection of classes useful for managing resources, including their loading and
 *        lifetimes
 */

#include <BLIB/Resources/FileSystem.hpp>
#include <BLIB/Resources/GarbageCollector.hpp>
#include <BLIB/Resources/Loader.hpp>
#include <BLIB/Resources/Resource.hpp>
#include <BLIB/Resources/ResourceManager.hpp>

#include <BLIB/Resources/Bundler.hpp>
#include <BLIB/Resources/Bundling/BundleMetadata.hpp>
#include <BLIB/Resources/Bundling/BundleSource.hpp>
#include <BLIB/Resources/Bundling/BundledFileMetadata.hpp>
#include <BLIB/Resources/Bundling/Config.hpp>
#include <BLIB/Resources/Bundling/FileHandler.hpp>
#include <BLIB/Resources/Bundling/FileHandlerContext.hpp>
#include <BLIB/Resources/Bundling/FileHandlers/AnimationHandler.hpp>
#include <BLIB/Resources/Bundling/FileHandlers/DefaultHandler.hpp>
#include <BLIB/Resources/Bundling/FileHandlers/PlaylistHandler.hpp>
#include <BLIB/Resources/Bundling/Manifest.hpp>

#endif
