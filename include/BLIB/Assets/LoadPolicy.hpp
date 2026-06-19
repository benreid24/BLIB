#ifndef BLIB_ASSETS_LOADPOLICY_HPP
#define BLIB_ASSETS_LOADPOLICY_HPP

namespace bl
{
namespace as
{
/**
 * @brief Defines the load policy for aseet payload dependencies
 *
 * @ingroup Assets
 */
enum struct LoadPolicy {
    /// Default. Dependencies are loaded immediately prior to loading an asset.
    Eager,

    /// Dependencies are loaded only when they are accessed.
    Lazy,

    /// Dependencies are never automatically loaded. Users must validate load state manually
    Manual
};

} // namespace as
} // namespace bl

#endif
