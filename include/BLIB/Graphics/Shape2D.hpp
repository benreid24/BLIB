#ifndef BLIB_GRAPHICS_SHAPE_HPP
#define BLIB_GRAPHICS_SHAPE_HPP

#include <BLIB/Components/Shape2D.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Base class for 2d shapes. Provides index buffer creation and management
 *
 * @ingroup Graphics
 */
class Shape2D
: public Drawable<com::Shape2D>
, public bcom::OverlayScalable {
public:
    /**
     * @brief Sets the color to fill the shape with
     *
     * @param color The color to fill the shape with
     */
    void setFillColor(const glm::vec4& color);

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

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;

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
     * @brief Call whenever the vertices need to be refreshed
     */
    void markDirty();

    /**
     * @brief Creates the ECS entity and required components
     *
     * @param engine The game engine instance
     */
    void create(engine::Engine& engine);

private:
    glm::vec4 fillColor;
    glm::vec4 outlineColor;
    float outlineThickness;
    sf::FloatRect localBounds;
    bool dirty;
    bool updateQueued;

    void update();
};

} // namespace gfx
} // namespace bl

#endif
