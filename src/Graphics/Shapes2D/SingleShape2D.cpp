#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>

namespace bl
{
namespace gfx
{
namespace s2d
{

SingleShape2D::SingleShape2D()
: Drawable()
, OverlayScalable()
, dirty(true) {}

SingleShape2D::~SingleShape2D() {
    if (updateHandle.isQueued()) { updateHandle.cancel(); }
}

void SingleShape2D::notifyDirty() {
    dirty = true;
    if (!updateHandle.isQueued() && entity() != ecs::InvalidEntity) {
        updateHandle = engine().systems().addFrameTask(
            engine::FrameStage::RendererDataSync, std::bind(&SingleShape2D::ensureUpdated, this));
    }
}

void SingleShape2D::create(engine::World& world) {
    Drawable::create(world);
    OverlayScalable::create(world.engine(), entity());
    notifyDirty();
}

void SingleShape2D::create(engine::World& world, ecs::Entity existing) {
    Drawable::createComponentOnly(world, existing);
    OverlayScalable::create(world.engine(), entity());
    notifyDirty();
}

void SingleShape2D::ensureLocalSizeUpdated() { ensureUpdated(); }

void SingleShape2D::commit() {
    ensureUpdated();
    updateHandle.cancel();
}

void SingleShape2D::ensureUpdated() {
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

void SingleShape2D::scaleToSize(const glm::vec2& size) {
    const sf::FloatRect& localBounds = getLocalBounds();
    getTransform().setScale({size.x / localBounds.width, size.y / localBounds.height});
    OverlayScalable::setLocalBounds(getLocalBounds());
}

} // namespace s2d
} // namespace gfx
} // namespace bl
