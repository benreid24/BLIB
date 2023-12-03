#ifndef BLIB_GRAPHICS_SHAPES2D_SHAPE2D_HPP
#define BLIB_GRAPHICS_SHAPES2D_SHAPE2D_HPP

#include <BLIB/Render/Primitives/Color.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace gfx
{
/// Collection of classes for rendering 2d shapes
namespace s2d
{
/**
 * @brief Base class for all 2d shapes. Provides part of the interface, as well as index buffer
 *        population. Does not provide storage
 *
 * @ingroup Graphics
 */
class Shape2D {
public:
    /**
     * @brief Destroys the shape
     */
    virtual ~Shape2D() = default;

    /**
     * @brief Sets the color to fill the shape with
     *
     * @param color The color to fill the shape with
     */
    void setFillColor(const glm::vec4& color);

    /**
     * @brief Sets the color to fill the shape with
     *
     * @param color The color to fill the shape with
     */
    void setFillColor(const sf::Color& color) { setFillColor(sfcol(color)); }

    /**
     * @brief Returns the color the shape is filled with
     */
    const glm::vec4& getFillColor() const;

    /**
     * @brief Sets the color the outline will be filled with
     *
     * @param color The color to outline the shape with
     */
    void setOutlineColor(const glm::vec4& color);

    /**
     * @brief Sets the color the outline will be filled with
     *
     * @param color The color to outline the shape with
     */
    void setOutlineColor(const sf::Color& color) { setOutlineColor(sfcol(color)); }

    /**
     * @brief Returns the color of the outline
     */
    const glm::vec4& getOutlineColor() const;

    /**
     * @brief Sets the thickness of the outline. Negative outline will render inside the bounds of
     *        the shape. Setting to 0 disables the outline
     *
     * @param thickness The thickness of the outline. In the same units as the shape size
     */
    void setOutlineThickness(float thickness);

    /**
     * @brief Returns the thickness of the outline
     */
    float getOutlineThickness() const;

    /**
     * @brief Returns the bounds of the shape. Bounds only computed if the shape is created first
     */
    const sf::FloatRect& getLocalBounds();

protected:
    /**
     * @brief Initializes the shapes fields to sane defaults
     */
    Shape2D();

    /**
     * @brief Derived classes should return the number of points their shape requires
     *
     * @return The number of points for the shape (ie 4 for rectangle, etc)
     */
    virtual unsigned int getVertexCount() const = 0;

    /**
     * @brief Derived classes should set the position (and may override the color) in this method.
     *        This will be called for indices [0, getVertexCount())
     *
     * @param index The index of the current point in the shape
     * @param vertex The vertex to populate
     */
    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) = 0;

    /**
     * @brief Allows derived classes to set the color of the center vertex. Simply return the
     *        computed average color if no explicit center color is desired (default behavior).
     *
     * @param avgColor The computed average color of all the vertices
     * @return The color of the center vertex that is managed by this class
     */
    virtual glm::vec4 getCenterColor(const glm::vec4& avgColor) const;

    /**
     * @brief Called whenever the vertices need to be refreshed
     */
    virtual void notifyDirty() = 0;

    /**
     * @brief Calculates and returns the number of vertices required for the shape
     */
    std::uint32_t calculateRequiredVertices() const;

    /**
     * @brief Calculates and returns the number of indices required for the shape
     */
    std::uint32_t calculateRequiredIndices() const;

    /**
     * @brief Updates the index buffer to render this shape. Storage should be large enough for
     *        calculated size
     *
     * @param vertices Pointer to the vertices for the shape
     * @param indices Pointer to the indices for the shape
     */
    void update(rc::prim::Vertex* vertices, std::uint32_t* indices);

    /**
     * @brief Called when local bounds is queried to ensure they are correct
     */
    virtual void ensureUpdated() = 0;

private:
    glm::vec4 fillColor;
    glm::vec4 outlineColor;
    float outlineThickness;
    sf::FloatRect localBounds;
};

} // namespace s2d
} // namespace gfx
} // namespace bl

#endif
