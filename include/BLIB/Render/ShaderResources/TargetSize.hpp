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

        /// Target size will be a multiple of the observer size
        ObserverSizeRatio,

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
    , ratio{} {}

    /**
     * @brief Creates the size control with the given type and size
     *
     * @param type The type of control
     * @param size The size to use
     */
    constexpr TargetSize(Type type, glm::u32vec2 size)
    : type(type)
    , size(size) {}

    /**
     * @brief Creates the size control with the given type and ratio
     *
     * @param type The type of control
     * @param ratio The ratio to use
     */
    constexpr TargetSize(Type type, glm::vec2 ratio)
    : type(type)
    , ratio(ratio) {}

    /**
     * @brief Copy constructor
     *
     * @param copy The value to copy
     */
    TargetSize(const TargetSize& copy)
    : type(copy.type) {
        switch (Type::FixedSize) {
        case Type::ObserverSizeRatio:
            ratio = copy.ratio;
            break;
        case Type::FixedSize:
            size = copy.size;
            break;
        case Type::ObserverSize:
        default:
            size = {};
            break;
        }
    }

    /**
     * @brief Helper method to get the size of an attachment from the size config
     *
     * @param observerSize The resolution of the observer space
     * @return The size to make the attachment
     */
    glm::u32vec2 getSize(glm::u32vec2 observerSize) const {
        switch (type) {
        case Type::FixedSize:
            return size;

        case Type::ObserverSizeRatio:
            return glm::u32vec2(glm::vec2(observerSize) * ratio);

        case Type::ObserverSize:
        default:
            return observerSize;
        }
    }

    Type type;
    union {
        glm::u32vec2 size;
        glm::vec2 ratio;
    };
};
} // namespace sri
} // namespace rc
} // namespace bl

#endif
