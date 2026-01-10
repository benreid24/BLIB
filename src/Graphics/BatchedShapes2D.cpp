#include <BLIB/Graphics/BatchedShapes2D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
void BatchedShapes2D::create(engine::World& world, std::uint32_t vc) {
    Drawable::create(world, world.engine().renderer(), vc, vc * 3);
    OverlayScalable::create(world.engine(), entity());
    component().commit();
}

void BatchedShapes2D::scaleToSize(const glm::vec2& size) {
    const sf::FloatRect bounds = computeLocalBounds();
    getTransform().setScale({size.x / bounds.width, size.y / bounds.height});
}

void BatchedShapes2D::ensureLocalSizeUpdated() {
    OverlayScalable::setLocalBounds(computeLocalBounds());
}

sf::FloatRect BatchedShapes2D::computeLocalBounds() const {
    sf::FloatRect bounds(0.f, 0.f, 0.f, 0.f);
    unsigned int i = 0;
    for (const auto& v : component().indexBuffer.getIndexBuffer().vertices()) {
        if (i >= component().indexBuffer.vertexCount()) { break; }
        bounds.left   = std::min(bounds.left, v.pos.x);
        bounds.top    = std::min(bounds.top, v.pos.y);
        bounds.width  = std::max(bounds.width, v.pos.x);
        bounds.height = std::max(bounds.height, v.pos.y);
        ++i;
    }
    bounds.width -= bounds.left;
    bounds.height -= bounds.top;
    return bounds;
}

} // namespace gfx
} // namespace bl
