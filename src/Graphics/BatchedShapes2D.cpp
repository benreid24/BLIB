#include <BLIB/Graphics/BatchedShapes2D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
void BatchedShapes2D::create(engine::Engine& engine, std::uint32_t vc) {
    Drawable::create(engine, engine.renderer().vulkanState(), vc, vc * 3);
    OverlayScalable::create(engine, entity());
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
    for (const auto& v : component().indexBuffer.getIndexBuffer().vertices()) {
        bounds.left   = std::min(bounds.left, v.pos.x);
        bounds.top    = std::min(bounds.top, v.pos.y);
        bounds.width  = std::max(bounds.width, v.pos.x);
        bounds.height = std::max(bounds.height, v.pos.y);
    }
    bounds.width -= bounds.left;
    bounds.height -= bounds.top;
    return bounds;
}

} // namespace gfx
} // namespace bl
