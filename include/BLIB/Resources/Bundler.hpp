#ifndef BLIB_RESOURCES_BUNDLER_HPP
#define BLIB_RESOURCES_BUNDLER_HPP

#include <BLIB/Resources/Bundling/Config.hpp>

namespace bl
{
namespace resource
{
/**
 * @brief Top level resource bundler class. Use this to create bundles for release builds
 *
 * @ingroup Resources
 * @ingroup Bundling
 */
class Bundler {
public:
    /**
     * @brief Construct a new Bundler from the given config
     *
     * @param config Config that describes how to create bundles
     */
    Bundler(bundle::Config&& config);

    /**
     * @brief Creates and saves all of the resource bundles described by the config
     *
     * @return True if bundles could be created, false on error
     */
    bool createBundles();

private:
    bundle::Config config;
};

} // namespace resource
} // namespace bl

#endif
