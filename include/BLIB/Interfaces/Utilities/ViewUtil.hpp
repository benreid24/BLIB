#ifndef BLIB_INTERFACES_UTILITIES_VIEWUTIL_HPP
#define BLIB_INTERFACES_UTILITIES_VIEWUTIL_HPP

#include <SFML/Graphics/View.hpp>

namespace bl
{
namespace interface
{
/**
 * @brief Collection of helper functions for computing views for various constraints. Most utilities
 *        are primarily for computing views to render menus.
 *
 * @ingroup InterfaceUtilities
 *
 */
struct ViewUtil {
    /**
     * @brief Represents which side to anchor a view to for anchored views
     *
     */
    enum Anchor { Top, Right, Left, Bottom };

    /**
     * @brief Helper function to compute a view of the given size that will cover the given area of
     *        the original view. This is most useful for creating views to render menus of fixed
     *        resolution that fill fixed areas while not having to be concerned with the original
     *        view area or size
     *
     * @param size The desired size of the computed view. Represents the renderable coordinate space
     * @param oldView The original view that is being replaced
     * @param coverArea The portion of the original viewport to fill. Values are normalized
     * @return sf::View The computed view to apply for proper rendering. Center is half the size
     */
    static sf::View computeView(const sf::Vector2f& size, const sf::View& oldView,
                                const sf::FloatRect& coverArea);

    /**
     * @brief Helper function to compute a view of the given size that will cover the given area of
     *        the original view. This is most useful for creating views to render menus of fixed
     *        resolution that fill fixed areas while not having to be concerned with the original
     *        view area or size
     *
     * @param region Coordinate space to cover with the view
     * @param defaultView The original view that is being replaced
     * @return sf::View The computed view to apply for proper rendering. Center is half the size
     */
    static sf::View computeSubView(const sf::FloatRect& region, const sf::View& defaultView);

    /**
     * @brief Helper function to compute a view of the given size that will cover the given area of
     *        the original view. This is most useful for creating views to render menus of fixed
     *        resolution that fill fixed areas while not having to be concerned with the original
     *        view area or size. This version automatically computes viewport height based on
     *        viewport width and the view size
     *
     * @param size The desired size of the computed view. Represents the renderable coordinate space
     * @param oldView The original view that is being replaced
     * @param viewportPosition The position in the original viewport, in range [0, 1]
     * @param widthRatio The width of the original viewport to fill, in range [0, 1]
     * @return sf::View The computed view to apply for proper rendering. Center is half the size
     */
    static sf::View computeViewPreserveAR(const sf::Vector2f& size, const sf::View& oldView,
                                          const sf::Vector2f& viewportPosition, float widthRatio);

    /**
     * @brief Computes a view that is anchored to the given side of the existing viewport. The non
     *        anchored dimension is centered on the corresponding dimension of the original viewport
     *
     * @param size The coordinate space of the result view
     * @param oldView The original view that is being anchored inside of
     * @param viewportSize The portion of the original viewport to fill
     * @param side Which side to anchor to
     * @return sf::View The view, anchored and positioned
     */
    static sf::View computeViewAnchored(const sf::Vector2f& size, const sf::View& oldView,
                                        const sf::Vector2f& viewportSize, Anchor side);

    /**
     * @brief Computes a view that is anchored to the given side of the existing viewport. The non
     *        anchored dimension is centered on the corresponding dimension of the original
     *        viewport. The resulting viewport height is computed based on the aspect ratio of the
     *        requested view size and the desired viewport width
     *
     * @param size The coordinate space of the result view
     * @param oldView The original view that is being anchored inside of
     * @param viewportWidth The portion of the original viewport width to fill. Height is computed
     * @param side Which side to anchor to
     * @return sf::View The view, anchored and positioned
     */
    static sf::View computeViewAnchoredPreserveAR(const sf::Vector2f& size, const sf::View& oldView,
                                                  float viewportWidth, Anchor side);

    /**
     * @brief Constrains the viewport of the given view to the area that overlaps with the viewport
     *        of the original view
     *
     * @param view The view to constrain
     * @param oldView The original view to constrain to
     */
    static void constrainView(sf::View& view, const sf::View& oldView);
};

} // namespace interface
} // namespace bl

#endif
