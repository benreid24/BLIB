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
    , size{}
    , ratio{} {}

    /**
     * @brief Creates the size control with the given type and size
     *
     * @param type The type of control
     * @param x The width to use
     * @param y The height to use
     */
    constexpr TargetSize(Type type, unsigned int x, unsigned int y)
    : type(type)
    , size{x, y}
    , ratio{} {}

    /**
     * @brief Creates the size control with the given type and ratio
     *
     * @param type The type of control
     * @param x The width ratio to use
     * @param y The height ratio to use
     */
    constexpr TargetSize(Type type, float x, float y)
    : type(type)
    , size{}
    , ratio{x, y} {}

    /**
     * @brief Copy constructor
     *
     * @param copy The value to copy
     */
    constexpr TargetSize(const TargetSize& copy)
    : type(copy.type)
    , size(copy.size)
    , ratio(copy.ratio) {}

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

        case Type::ObserverSizeRatio:
            return glm::u32vec2(glm::vec2(observerSize) * glm::vec2(ts.ratio.x, ts.ratio.y));

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

    struct Ratio {
        float x;
        float y;
    } ratio;
};
} // namespace sri
} // namespace rc
} // namespace bl

#endif
