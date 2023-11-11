#include <BLIB/Graphics/Shapes2D/BatchShape2D.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Graphics/BatchedShapes2D.hpp>

namespace bl
{
namespace gfx
{
namespace s2d
{
BatchShape2D::BatchShape2D()
: Shape2D()
, owner(nullptr)
, dirty(false) {}

void BatchShape2D::create(engine::Engine& e, BatchedShapes2D& o) {
    engine = &e;
    owner  = &o;
    notifyDirty();
}

void BatchShape2D::notifyDirty() {
    dirty = true;
    if (!updateHandle.isQueued() && owner != nullptr) {
        updateHandle = engine->systems().addFrameTask(
            engine::FrameStage::RenderObjectSync, std::bind(&BatchShape2D::ensureUpdated, this));
    }
}

void BatchShape2D::ensureUpdated() {
    if (dirty && owner) {
        dirty = false;

        // determine required vertex and index counts
        const std::uint32_t vertexCount = calculateRequiredVertices();
        const std::uint32_t indexCount  = calculateRequiredIndices();

        // (re)alloc if necessary
        if (!alloc.isValid() || alloc.getInfo().indexSize != indexCount ||
            alloc.getInfo().vertexSize != vertexCount) {
            alloc = owner->component().indexBuffer.allocate(vertexCount, indexCount);
        }

        // populate vertices
        update(alloc.getVertices(), alloc.getIndices());
        alloc.commit();

        // refresh owner bounds and draw call
        owner->ensureLocalSizeUpdated();
        owner->component().commit();
    }
}

} // namespace s2d
} // namespace gfx
} // namespace bl
