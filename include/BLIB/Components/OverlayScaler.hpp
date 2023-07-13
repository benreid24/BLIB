#ifndef BLIB_COMPONENTS_OVERLAYSCALER_HPP
#define BLIB_COMPONENTS_OVERLAYSCALER_HPP

#include <SFML/Graphics/Rect.hpp>
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
 * @ingroup Graphics
 */
class OverlayScaler {
public:
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
     * @brief Sets the pre-transform size of the entity. Used in scaling calculations
     *
     * @param size The pre-transform size of the entity
     */
    void setEntitySize(const glm::vec2& size);

    /**
     * @brief Returns the pre-transform size of the entity
     */
    constexpr const glm::vec2& getEntitySize() const;

    /**
     * @brief Returns whether or not the scale needs to be re-computed
     */
    constexpr bool isDirty() const;

private:
    enum ScaleType { None, WidthPercent, HeightPercent, SizePercent, PixelRatio, LineHeight };

    sf::FloatRect cachedTargetRegion;
    glm::vec2 cachedObjectSize;
    ScaleType scaleType;
    union {
        float pixelRatio;
        float widthPercent;
        float heightPercent;
        glm::vec2 sizePercent;
        float overlayRatio;
    };
    bool useViewport;
    std::optional<glm::vec2> ogPos;
    bool dirty;

    void setViewportToSelf(bool setToSelf);

    friend class sys::OverlayScalerSystem;
    friend class gfx::bcom::OverlayScalable;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const glm::vec2& OverlayScaler::getEntitySize() const { return cachedObjectSize; }

inline constexpr bool OverlayScaler::isDirty() const { return dirty && scaleType != None; }

} // namespace com
} // namespace bl

#endif
