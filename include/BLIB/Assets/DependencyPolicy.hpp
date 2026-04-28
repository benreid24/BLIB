#ifndef BLIB_ASSETS_DEPENDENCYPOLICY_HPP
#define BLIB_ASSETS_DEPENDENCYPOLICY_HPP

namespace bl
{
namespace as
{
/**
 * @brief Represents whether a dependency is required or optional. Optional dependencies need to be
 *        checked prior to access as they may be missing or in failed states
 *
 * @ingroup Assets
 */
enum struct DependencyPolicy {
    /// The dependency is required and must be loaded for the asset to be valid
    Required,

    /// The dependency is optional and may be missing or failed
    Optional
};

} // namespace as
} // namespace bl

#endif
