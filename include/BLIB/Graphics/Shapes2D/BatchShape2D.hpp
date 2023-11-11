#ifndef BLIB_GRAPHICS_SHAPES2D_BATCHSHAPE2D_HPP
#define BLIB_GRAPHICS_SHAPES2D_BATCHSHAPE2D_HPP

#include <BLIB/Engine/Systems.hpp>
#include <BLIB/Graphics/Shapes2D/Shape2D.hpp>
#include <BLIB/Render/Buffers/BatchIndexBuffer.hpp>

namespace bl
{
namespace gfx
{
class BatchedShapes2D;

namespace s2d
{
/**
 * @brief Base class for batched 2d shapes
 *
 * @ingroup Graphics
 */
class BatchShape2D : public Shape2D {
public:
    /**
     * @brief Destroys the shape
     */
    virtual ~BatchShape2D() = default;

protected:
    /**
     * @brief Initializes the shape
     */
    BatchShape2D();

    /**
     * @brief Sets the owner and allocates the required vertices
     *
     * @param engine The game engine instance
     * @param owner The batch of shapes to render with
     */
    void create(engine::Engine& engine, BatchedShapes2D& owner);

    /**
     * @brief Call whenever the shape geometry needs to be updated
     */
    virtual void notifyDirty() override;

private:
    engine::Engine* engine;
    BatchedShapes2D* owner;
    rc::buf::BatchIndexBuffer::AllocHandle alloc;
    bool dirty;
    engine::Systems::TaskHandle updateHandle;
    // TODO - transform + hook to apply

    virtual void ensureUpdated() override;
};
} // namespace s2d
} // namespace gfx
} // namespace bl

#endif
