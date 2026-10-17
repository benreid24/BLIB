#include <BLIB/Objects/2D/Shapes/BatchShape.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Objects/2D/BatchedShapes.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
namespace s2d
{
BatchShape::BatchShape()
: Shape()
, owner(nullptr)
, dirty(false) {}

BatchShape::~BatchShape() {
    remove();
    if (updateHandle.isQueued()) { updateHandle.cancel(); }
}

void BatchShape::create(engine::Engine& e, BatchedShapes& o) {
    engine = &e;
    owner  = &o;
    notifyDirty();
}

void BatchShape::notifyDirty() {
    dirty = true;
    if (!updateHandle.isQueued() && owner != nullptr) {
        updateHandle = engine->systems().addFrameTask(engine::FrameStage::RendererDataSync,
                                                      std::bind(&BatchShape::ensureUpdated, this));
    }
}

com::Transform2D& BatchShape::getLocalTransform() {
    notifyDirty();
    return transform;
}

void BatchShape::commit() {
    if (dirty && owner) {
        dirty = false;
        updateHandle.cancel();

        updateVertices();
        alloc.commit();
        owner->component().commit();
    }
}

void BatchShape::ensureUpdated() {
    if (dirty && owner) {
        dirty = false;

        updateVertices();

        // refresh owner bounds and draw call
        alloc.commit();
        owner->ensureLocalSizeUpdated();
        owner->component().commit();
    }
}

void BatchShape::updateVertices() {
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

void BatchShape::remove() {
    if (alloc.isValid()) {
        if (alloc.release() && owner) {
            owner->ensureLocalSizeUpdated();
            owner->component().commit();
            owner = nullptr;
        }
    }
}

bool BatchShape::isCreated() const { return alloc.isValid(); }

} // namespace s2d
} // namespace o2d
} // namespace obj
} // namespace bl
