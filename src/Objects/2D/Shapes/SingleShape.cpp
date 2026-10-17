#include <BLIB/Objects/2D/Shapes/SingleShape.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
namespace s2d
{

SingleShape::SingleShape()
: DrawableObject2D()
, dirty(true) {}

SingleShape::~SingleShape() {
    if (updateHandle.isQueued()) { updateHandle.cancel(); }
}

void SingleShape::notifyDirty() {
    dirty = true;
    if (!updateHandle.isQueued() && entity() != ecs::InvalidEntity) {
        updateHandle = engine().systems().addFrameTask(
            engine::FrameStage::RendererDataSync, std::bind(&SingleShape::ensureUpdated, this));
    }
}

void SingleShape::create(engine::World& world) {
    DrawableObject2D::create(world);
    ensureUpdated();
}

void SingleShape::create(engine::World& world, ecs::Entity existing) {
    DrawableObject2D::createComponentOnly(world, existing);
    ensureUpdated();
}

void SingleShape::ensureLocalSizeUpdated() { ensureUpdated(); }

void SingleShape::commit() {
    ensureUpdated();
    updateHandle.cancel();
}

void SingleShape::ensureUpdated() {
    if (entity() == ecs::InvalidEntity || !dirty) { return; }
    dirty = false;

    // determine required vertex and index counts
    const std::uint32_t vertexCount = calculateRequiredVertices();
    const std::uint32_t indexCount  = calculateRequiredIndices();

    // create the index buffer
    auto& ib = component().indexBuffer;
    if (ib.indexCount() < indexCount || ib.vertexCount() < vertexCount) {
        ib.create(engine().renderer(), vertexCount, indexCount);
    }

    // populate the index buffer
    update(ib.vertices().data(), ib.indices().data());

    // determine if we are transparent
    bool t = false;
    for (const auto& v : ib.vertices()) {
        if (v.color.a > 0.01f && v.color.a < 0.99f) {
            t = true;
            break;
        }
    }
    component().setContainsTransparency(t);

    // Update local size and commit index buffer + draw command
    OverlayScalable::setLocalBounds(getLocalBounds());
    component().commit();
}

void SingleShape::scaleToSize(const glm::vec2& size) {
    const sf::FloatRect& localBounds = getLocalBounds();
    getTransform().setScale({size.x / localBounds.size.x, size.y / localBounds.size.y});
    OverlayScalable::setLocalBounds(getLocalBounds());
}

} // namespace s2d
} // namespace o2d
} // namespace obj
} // namespace bl
