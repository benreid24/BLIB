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
    getTransform().setScale({size.x / bounds.size.x, size.y / bounds.size.y});
}

void BatchedShapes2D::ensureLocalSizeUpdated() {
    OverlayScalable::setLocalBounds(computeLocalBounds());
}

sf::FloatRect BatchedShapes2D::computeLocalBounds() const {
    sf::FloatRect bounds({0.f, 0.f}, {0.f, 0.f});
    unsigned int i = 0;
    for (const auto& v : component().indexBuffer.getIndexBuffer().vertices()) {
        if (i >= component().indexBuffer.vertexCount()) { break; }
        bounds.position.x = std::min(bounds.position.x, v.pos.x);
        bounds.position.y = std::min(bounds.position.y, v.pos.y);
        bounds.size.x     = std::max(bounds.size.x, v.pos.x);
        bounds.size.y     = std::max(bounds.size.y, v.pos.y);
        ++i;
    }
    bounds.size.x -= bounds.position.x;
    bounds.size.y -= bounds.position.y;
    return bounds;
}

} // namespace gfx
} // namespace bl
