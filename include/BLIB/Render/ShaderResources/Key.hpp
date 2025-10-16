#ifndef BLIB_RENDER_SHADERRESOURCES_KEY_HPP
#define BLIB_RENDER_SHADERRESOURCES_KEY_HPP

#include <string_view>

namespace bl
{
namespace rc
{
namespace sr
{
/**
 * @brief Key type for a shader resource. Allows encoding both type and id into one value
 *
 * @tparam T The type of resource being identified
 * @ingroup Renderer
 */
template<typename T>
struct Key {
    using TResource = T;

    /**
     * @brief Creates a key
     *
     * @param id The id of the resource
     */
    constexpr Key(std::string_view id)
    : id(id) {}

    const std::string_view id;
};

} // namespace sr
} // namespace rc
} // namespace bl

#endif
