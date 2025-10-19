#ifndef BLIB_RENDER_SHADERRESOURCES_STOREKEY_HPP
#define BLIB_RENDER_SHADERRESOURCES_STOREKEY_HPP

namespace bl
{
namespace rc
{
namespace sr
{
/**
 * @brief Enum representing shader resource stores
 *
 * @ingroup Renderer
 */
enum struct StoreKey {
    /// The resource store global to the renderer
    Global,

    /// The resource store for an observer and all of its scenes
    Observer,

    /// The resource store specific to a scene/observer pair
    Scene
};

} // namespace sr
} // namespace rc
} // namespace bl

#endif
