#ifndef BLIB_RENDER_SHADERRESOURCES_TARGETSIZE_HPP
#define BLIB_RENDER_SHADERRESOURCES_TARGETSIZE_HPP

#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
/**
 * @brief Basic struct defining the size of an offscreen target
 *
 * @ingroup Renderer
 */
struct TargetSize {
    /// The behavior of the target size
    enum Type {
        /// Target size will match observer size directly
        ObserverSize,

        /// Target size will be a fixed size
        FixedSize
    };

    /**
     * @brief Creates the size control with the given type
     *
     * @param type The type of control
     */
    constexpr TargetSize(Type type = ObserverSize)
    : type(type)
    , size{} {}

    /**
     * @brief Creates the size control with the given type and size
     *
     * @param type The type of control
     * @param x The width to use
     * @param y The height to use
     */
    constexpr TargetSize(Type type, unsigned int x, unsigned int y)
    : type(type)
    , size{x, y} {}

    /**
     * @brief Copy constructor
     *
     * @param copy The value to copy
     */
    constexpr TargetSize(const TargetSize& copy) = default;

    /**
     * @brief Equality operator
     */
    constexpr bool operator==(const TargetSize& comp) const {
        return type == comp.type && size.x == comp.size.x && size.y == comp.size.y;
    }

    /**
     * @brief Helper method to get the size of an attachment from the size config
     *
     * @param observerSize The resolution of the observer space
     * @return The size to make the attachment
     */
    static glm::u32vec2 getSize(TargetSize ts, glm::u32vec2 observerSize) {
        switch (ts.type) {
        case Type::FixedSize:
            return {ts.size.x, ts.size.y};

        case Type::ObserverSize:
        default:
            return observerSize;
        }
    }

    Type type;
    struct Size {
        unsigned int x;
        unsigned int y;
    } size;
};
} // namespace sri
} // namespace rc
} // namespace bl

#endif
