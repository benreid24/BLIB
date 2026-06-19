#ifndef BLIB_ASSETS_BUNDLES_ASSETBUNDLECONFIG_HPP
#define BLIB_ASSETS_BUNDLES_ASSETBUNDLECONFIG_HPP

namespace bl
{
namespace as
{
namespace bdl
{
/**
 * @brief Basic struct representing asset bundle behavior. Provided by drivers per asset type
 *
 * @ingroup Assets
 */
struct AssetBundleConfig {
    /// Enum representing asset bundle affinity
    enum struct Affinity {
        /// Assets will try to be bundled with their parent
        Parent,

        /// Assets will be bundled together by type
        Type
    };

    /// Enum that determines how assets are selected for bundling
    enum struct Selection {
        /// Assets will comprise bundle roots
        Root,

        /// Assets will be bundled following their affinity
        NonRoot,

        /// Assets will only be included if they are depended on by others
        IfNeeded
    };

    /// Enum representing whether assets are autoloaded on bundle mount or not
    enum struct OnMount {
        /// Assets are autoloaded when the bundle containing them is mounted
        AutoLoad,

        /// Assets are only loaded when directly requested or if autoload is enabled
        WhenRequested
    };

    Affinity affinity   = Affinity::Parent;
    Selection selection = Selection::NonRoot;
    OnMount onMount     = OnMount::WhenRequested;
};

} // namespace bdl
} // namespace as
} // namespace bl

#endif
