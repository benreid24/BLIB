#ifndef BLIB_RENDER_GRAPH_ASSETS_TARGETSIZE_HPP
#define BLIB_RENDER_GRAPH_ASSETS_TARGETSIZE_HPP

namespace bl
{
namespace rc
{
namespace rgi
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
    TargetSize(Type type = ObserverSize)
    : type(type)
    , ratio{} {}

    /**
     * @brief Creates the size control with the given type and size
     *
     * @param type The type of control
     * @param size The size to use
     */
    TargetSize(Type type, glm::u32vec2 size)
    : type(type)
    , size(size) {}

    /**
     * @brief Creates the size control with the given type and ratio
     *
     * @param type The type of control
     * @param ratio The ratio to use
     */
    TargetSize(Type type, glm::vec2 ratio)
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
        case bl::rc::rgi::TargetSize::ObserverSizeRatio:
            ratio = copy.ratio;
            break;
        case bl::rc::rgi::TargetSize::FixedSize:
            size = copy.size;
            break;
        case bl::rc::rgi::TargetSize::ObserverSize:
        default:
            size = {};
            break;
        }
    }

    Type type;
    union {
        glm::u32vec2 size;
        glm::vec2 ratio;
    };
};
} // namespace rgi
} // namespace rc
} // namespace bl

#endif
