#ifndef BLIB_OBJECTS_2D_SHAPES_SINGLESHAPE2D_HPP
#define BLIB_OBJECTS_2D_SHAPES_SINGLESHAPE2D_HPP

#include <BLIB/Components/Shape2D.hpp>
#include <BLIB/Engine/Systems.hpp>
#include <BLIB/Objects/2D/DrawableObject2D.hpp>
#include <BLIB/Objects/2D/Shapes/Shape.hpp>
#include <BLIB/Objects/Components/OverlayScalable.hpp>
#include <BLIB/Objects/Components/Transform2D.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
namespace s2d
{
/**
 * @brief Base class for individual 2d shapes. Provides index buffer creation and management
 *
 * @ingroup Objects
 */
class SingleShape
: public Shape
, public DrawableObject2D<com::Shape2D> {
public:
    /**
     * @brief Destroys the shape
     */
    virtual ~SingleShape();

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;

    /**
     * @brief Does not need to be called manually, but should be called if this object is being used
     *        to create ECS components and then being destroyed immediately
     */
    void commit();

protected:
    /**
     * @brief Initializes the shapes fields to sane defaults
     */
    SingleShape();

    /**
     * @brief Creates the ECS entity and required components
     *
     * @param world The world to create the object in
     */
    void create(engine::World& world);

    /**
     * @brief Creates the required components on the existing entity
     *
     * @param world The world to create the object in
     * @param existingEntity The entity to add the sprite components to
     */
    void create(engine::World& world, ecs::Entity existingEntity);

    /**
     * @brief Called when the local size is queried
     */
    virtual void ensureLocalSizeUpdated() override;

    /**
     * @brief Call whenever the shape geometry needs to be updated
     */
    virtual void notifyDirty() override;

private:
    bool dirty;
    engine::Systems::TaskHandle updateHandle;

    virtual void ensureUpdated() override;
};

} // namespace s2d
} // namespace o2d
} // namespace obj
} // namespace bl

#endif
