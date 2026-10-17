#ifndef BLIB_OBJECTS_2D_BATCHEDSHAPES_HPP
#define BLIB_OBJECTS_2D_BATCHEDSHAPES_HPP

#include <BLIB/Components/BatchedShapes2D.hpp>
#include <BLIB/Objects/2D/Shapes/BatchShape.hpp>
#include <BLIB/Objects/2D/Shapes/Shape.hpp>
#include <BLIB/Objects/Components/OverlayScalable.hpp>
#include <BLIB/Objects/Drawable.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Drawable for a collection of batched 2d shapes. Create one of these and use with multiple
 *        BatchedCircles, BatchedRectangles, and BatchedTriangles
 *
 * @ingroup Objects
 */
class BatchedShapes
: public Drawable<com::BatchedShapes2D>
, public bcom::OverlayScalable {
public:
    /**
     * @brief Initializes with sane defaults
     */
    BatchedShapes() = default;

    /**
     * @brief Destroys the shapes
     */
    virtual ~BatchedShapes() = default;

    /**
     * @brief Creates the ECS entity and required components
     *
     * @param world The world to create the object in
     * @param vertexCapacity The number of vertices to initially allocate
     */
    void create(engine::World& world, std::uint32_t vertexCapacity);

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

    friend class s2d::BatchShape;
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
