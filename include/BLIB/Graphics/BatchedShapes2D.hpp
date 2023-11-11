#ifndef BLIB_GRAPHICS_BATCHEDSHAPES2D_HPP
#define BLIB_GRAPHICS_BATCHEDSHAPES2D_HPP

#include <BLIB/Components/BatchedShapes2D.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Graphics/Drawable.hpp>
#include <BLIB/Graphics/Shapes2D/Shape2D.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Drawable for a collection of batched 2d shapes. Create one of these and use with multiple
 *        BatchedCircles, BatchedRectangles, and BatchedTriangles
 *
 * @ingroup Graphics
 */
class BatchedShapes2D
: public Drawable<com::BatchedShapes2D>
, public bcom::OverlayScalable {
public:
    /**
     * @brief Initializes with sane defaults
     */
    BatchedShapes2D() = default;

    /**
     * @brief Destroys the shapes
     */
    virtual ~BatchedShapes2D() = default;

    /**
     * @brief Creates the ECS entity and required components
     *
     * @param engine The game engine instance
     * @param vertexCapacity The number of vertices to initially allocate
     */
    void create(engine::Engine& engine, std::uint32_t vertexCapacity);

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;

    /**
     * @brief Traverses all vertices and returns the local bounds
     */
    sf::FloatRect computeLocalBounds() const;

private:
    virtual void ensureLocalSizeUpdated() override;
};

} // namespace gfx
} // namespace bl

#endif
