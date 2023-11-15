#ifndef BLIB_COMPONENTS_OVERLAYSCALER_HPP
#define BLIB_COMPONENTS_OVERLAYSCALER_HPP

#include <BLIB/ECS/Traits/IgnoresDummy.hpp>
#include <BLIB/ECS/Traits/ParentAware.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <functional>
#include <glm/glm.hpp>
#include <optional>

namespace bl
{
namespace sys
{
class OverlayScalerSystem;
}

namespace gfx
{
namespace bcom
{
class OverlayScalable;
}
} // namespace gfx

namespace rc
{
class Overlay;
}

namespace com
{
/**
 * @brief Component used to scale entities in overlays according to fixed rules. sys::Overlay scaler
 *        ensures that scales are up-to-date as the overlay changes size. Entity transforms should
 *        not be manually scaled if an OverlayScaler is present and active
 *
 * @ingroup Components
 */
class OverlayScaler
: public ecs::trait::ParentAware<OverlayScaler>
, public ecs::trait::IgnoresDummy {
public:
    using OnScale = std::function<void()>;

    /**
     * @brief Represents the scissor behavior for an entity controlled by an overlay scaler
     */
    enum ScissorMode {
        /// Default setting. Uses same scissor as parent, or Observer scissor for root elements
        ScissorInherit,

        /// Sets the scissor to the bounds of the object
        ScissorSelf,

        /// Sets the scissor to the bounds of the object and constrains against the parent scissor
        ScissorSelfConstrained,

        /// Sets the scissor to the top-level observer scissor
        ScissorObserver
    };

    /**
     * @brief Initializes the component to have no scaling effect
     */
    OverlayScaler();

    /**
     * @brief Scales the entity to take up a percentage of the overlay width. Maintains aspect ratio
     *
     * @param percent Percentage of the overlay width to size to, in range [0, 1]
     */
    void scaleToWidthPercent(float percent);

    /**
     * @brief Scales the entity to take up a percentage of the overlay height. Maintains AR
     *
     * @param percent Percentage of the overlay height to size to, in range [0, 1]
     */
    void scaleToHeightPercent(float percent);

    /**
     * @brief Scales so that the given local height corresponds to the given overlay height. Used
     *        for Text scaling
     *
     * @param localLineHeight The line height to scale to overlay units
     * @param overlayLineHeight Overlay height units to scale lines to
     */
    void scaleToHeightRatio(float localLineHeight, float overlayLineHeight);

    /**
     * @brief Scales the entity to take up a percentage of the overlay size. Does not maintain AR
     *
     * @param percent Percentages of the overlay size to size to, in range [0, 1]
     */
    void scaleToSizePercent(const glm::vec2& percents);

    /**
     * @brief Scales to the given ratio of entity-local texels to target screen-space pixels
     *
     * @param ratio Linear multiplier to use to map from texel to pixel
     */
    void mapToTargetPixels(float ratio = 1.f);

    /**
     * @brief Disables auto-scaling
     */
    void stopScaling();

    /**
     * @brief Sets the pre-transform bounds of the entity. Used in scaling calculations
     *
     * @param bounds The pre-transform bounds of the entity
     */
    void setEntityBounds(const sf::FloatRect& bounds);

    /**
     * @brief Returns the pre-transform size of the entity
     */
    constexpr const sf::FloatRect& getEntityBounds() const;

    /**
     * @brief Sets the behavior of the scissor when rendering this entity
     *
     * @param mode The scissor mode
     */
    void setScissorMode(ScissorMode mode);

    /**
     * @brief Updates the entity position relative to the parent region
     *
     * @param position Normalized coordinates to position at
     */
    void positionInParentSpace(const glm::vec2& position);

    /**
     * @brief Stops any positioning of the entity
     */
    void stopPositioning();

    /**
     * @brief Returns whether or not the scale needs to be re-computed
     */
    constexpr bool isDirty() const;

    /**
     * @brief Sets the callback to call when the entity is scaled
     *
     * @param onScale The callback to call when the entity is scaled
     */
    void setScaleCallback(OnScale&& onScale);

private:
    enum ScaleType { None, WidthPercent, HeightPercent, SizePercent, PixelRatio, LineHeight };
    enum PositionType { NoPosition, ParentSpace };

    OnScale onScale;
    sf::FloatRect cachedObjectBounds;
    sf::FloatRect cachedTargetRegion;
    ScaleType scaleType;
    union {
        float pixelRatio;
        float widthPercent;
        float heightPercent;
        glm::vec2 sizePercent;
        float overlayRatio;
    };
    PositionType posType;
    union {
        glm::vec2 parentPosition;
    };
    ScissorMode scissorMode;
    bool dirty;

    friend class sys::OverlayScalerSystem;
    friend class gfx::bcom::OverlayScalable;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const sf::FloatRect& OverlayScaler::getEntityBounds() const {
    return cachedObjectBounds;
}

inline constexpr bool OverlayScaler::isDirty() const { return dirty; }

} // namespace com
} // namespace bl

#endif
