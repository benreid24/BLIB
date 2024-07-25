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

BatchShape2D::~BatchShape2D() {
    remove();
    if (updateHandle.isQueued()) { updateHandle.cancel(); }
}

void BatchShape2D::create(engine::Engine& e, BatchedShapes2D& o) {
    engine = &e;
    owner  = &o;
    notifyDirty();
}

void BatchShape2D::notifyDirty() {
    dirty = true;
    if (!updateHandle.isQueued() && owner != nullptr) {
        updateHandle = engine->systems().addFrameTask(
            engine::FrameStage::RenderEarlyRefresh, std::bind(&BatchShape2D::ensureUpdated, this));
    }
}

com::Transform2D& BatchShape2D::getLocalTransform() {
    notifyDirty();
    return transform;
}

void BatchShape2D::commit() {
    if (dirty && owner) {
        dirty = false;
        updateHandle.cancel();

        updateVertices();
        alloc.commit();
        owner->component().commit();
    }
}

void BatchShape2D::ensureUpdated() {
    if (dirty && owner) {
        dirty = false;

        updateVertices();

        // refresh owner bounds and draw call
        alloc.commit();
        owner->ensureLocalSizeUpdated();
        owner->component().commit();
    }
}

void BatchShape2D::updateVertices() {
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

    // offset indices
    for (std::uint32_t i = 0; i < indexCount; ++i) {
        alloc.getIndices()[i] += alloc.getInfo().vertexStart;
    }

    // apply transform to vertices
    for (std::uint32_t i = 0; i < vertexCount; ++i) {
        auto& v = alloc.getVertices()[i];
        v.pos   = transform.transformPoint(v.pos);
    }
}

void BatchShape2D::remove() {
    if (alloc.isValid()) {
        if (alloc.release() && owner) {
            owner->ensureLocalSizeUpdated();
            owner->component().commit();
            owner = nullptr;
        }
    }
}

bool BatchShape2D::isCreated() const { return alloc.isValid(); }

} // namespace s2d
} // namespace gfx
} // namespace bl
