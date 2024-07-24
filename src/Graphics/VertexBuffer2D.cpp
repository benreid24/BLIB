#include <BLIB/Graphics/VertexBuffer2D.hpp>

namespace bl
{
namespace gfx
{
void VertexBuffer2D::create(engine::Engine& engine, unsigned int vc) {
    Drawable::create(engine);
    component().create(engine, vc);
}

void VertexBuffer2D::create(engine::Engine& engine, unsigned int vc, rc::res::TextureRef txtr) {
    create(engine, vc);
    Textured::create(engine.ecs(), entity(), txtr);
}

void VertexBuffer2D::scaleToSize(const glm::vec2& size) {
    if (getSize() > 0) {
        constexpr float MinF = std::numeric_limits<float>::min();
        constexpr float MaxF = std::numeric_limits<float>::max();
        sf::FloatRect bounds(MaxF, MaxF, MinF, MinF);

        for (unsigned int i = 0; i < component().getSize(); ++i) {
            const auto& v = component()[i];
            bounds.left   = std::min(v.pos.x, bounds.left);
            bounds.top    = std::min(v.pos.y, bounds.top);
            bounds.width  = std::max(v.pos.x, bounds.width);
            bounds.height = std::max(v.pos.y, bounds.height);
        }

        getTransform().setScale((bounds.width - bounds.left) / size.x,
                                (bounds.height - bounds.top) / size.y);
    }
}

} // namespace gfx
} // namespace bl
