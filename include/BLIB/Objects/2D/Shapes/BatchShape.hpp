#ifndef BLIB_OBJECTS_2D_SHAPES_BATCHSHAPE_HPP
#define BLIB_OBJECTS_2D_SHAPES_BATCHSHAPE_HPP

#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Engine/Systems.hpp>
#include <BLIB/Objects/2D/Shapes/Shape.hpp>
#include <BLIB/Render/Buffers/BatchIndexBuffer.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
class BatchedShapes;

namespace s2d
{
/**
 * @brief Base class for batched 2d shapes
 *
 * @ingroup Objects
 */
class BatchShape : public Shape {
public:
    /**
     * @brief Destroys the shape
     */
    virtual ~BatchShape();

    /**
     * @brief Returns the local transform of the shape. Determines the positions of the vertices
     *        within the batch
     */
    const com::Transform2D& getLocalTransform() const { return transform; }

    /**
     * @brief Returns the local transform of the shape. Determines the positions of the vertices
     *        within the batch. This override assumes the transform is modified and marks dirty
     */
    com::Transform2D& getLocalTransform();

    /**
     * @brief Removes the shape from the batch
     */
    void remove();

    /**
     * @brief Returns whether or not the shape has been created
     */
    bool isCreated() const;

    /**
     * @brief Call to manually commit geometry updates to the owning batch
     */
    void commit();

protected:
    /**
     * @brief Initializes the shape
     */
    BatchShape();

    /**
     * @brief Sets the owner and allocates the required vertices
     *
     * @param engine The game engine instance
     * @param owner The batch of shapes to render with
     */
    void create(engine::Engine& engine, BatchedShapes& owner);

    /**
     * @brief Call whenever the shape geometry needs to be updated
     */
    virtual void notifyDirty() override;

private:
    engine::Engine* engine;
    BatchedShapes* owner;
    com::Transform2D transform;
    rc::buf::BatchIndexBuffer::AllocHandle alloc;
    bool dirty;
    engine::Systems::TaskHandle updateHandle;

    virtual void ensureUpdated() override;
    void updateVertices();
};

} // namespace s2d
} // namespace o2d
} // namespace obj
} // namespace bl

#endif
