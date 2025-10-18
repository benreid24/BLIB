#ifndef BLIB_RENDER_SHADERRESOURCES_KEY_HPP
#define BLIB_RENDER_SHADERRESOURCES_KEY_HPP

#include <cstddef>

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
    template<std::size_t N>
    constexpr Key(const char (&id)[N]) {
        static_assert(N < MaxKeyLen, "Key id is too long");
        for (std::size_t i = 0; i < MaxKeyLen; ++i) { this->id[i] = '\0'; }
        for (std::size_t i = 0; i < N; ++i) { this->id[i] = id[i]; }
    }

    static constexpr std::size_t MaxKeyLen = 64;

    char id[MaxKeyLen];
};

} // namespace sr
} // namespace rc
} // namespace bl

#endif
