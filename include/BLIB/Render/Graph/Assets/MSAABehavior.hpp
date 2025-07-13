#ifndef BLIB_RENDER_GRAPH_ASSETS_MSAABEHAVIOR_HPP
#define BLIB_RENDER_GRAPH_ASSETS_MSAABEHAVIOR_HPP

#include <type_traits>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Controls the MSAA behavior of an asset
 *
 * @ingroup Renderer
 */
enum struct MSAABehavior {
    /// MSAA is disabled always for this asset
    Disabled = 0,

    /// MSAA is enabled or disabled based on the renderer settings
    UseSettings = 0x1 << 1,

    /// Attachments should be resolved after rendering
    ResolveAttachments = 0x1 << 2
};

} // namespace rgi
} // namespace rc
} // namespace bl

/**
 * @brief Helper to combine two MSAABehavior flags using bitwise OR
 *
 * @param a The first MSAABehavior flag
 * @param b The second MSAABehavior flag
 * @return The combined MSAABehavior flag
 */
inline constexpr bl::rc::rgi::MSAABehavior operator|(bl::rc::rgi::MSAABehavior a,
                                                     bl::rc::rgi::MSAABehavior b) {
    using T = std::underlying_type<bl::rc::rgi::MSAABehavior>::type;
    return static_cast<bl::rc::rgi::MSAABehavior>(static_cast<T>(a) | static_cast<T>(b));
}

/**
 * @brief Helper to check if a MSAABehavior flag is set
 *
 * @param a The MSAABehavior flag to check
 * @param b The MSAABehavior flag to check against
 * @return True if the flag is set, false otherwise
 */
inline constexpr bool operator&(bl::rc::rgi::MSAABehavior a, bl::rc::rgi::MSAABehavior b) {
    using T = std::underlying_type<bl::rc::rgi::MSAABehavior>::type;
    return (static_cast<T>(a) & static_cast<T>(b)) != 0;
}

#endif
