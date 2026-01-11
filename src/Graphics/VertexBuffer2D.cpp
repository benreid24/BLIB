#include <BLIB/Graphics/VertexBuffer2D.hpp>

namespace bl
{
namespace gfx
{
void VertexBuffer2D::create(engine::World& world, unsigned int vc) {
    Drawable::create(world);
    Transform2D::create(world.engine().ecs(), entity());
    component().create(world.engine(), vc);
}

void VertexBuffer2D::create(engine::World& world, unsigned int vc, rc::res::TextureRef txtr) {
    create(world, vc);
    Textured::create(world.engine().renderer(), &material(), txtr);
}

void VertexBuffer2D::scaleToSize(const glm::vec2& size) {
    if (getSize() > 0) {
        constexpr float MinF = std::numeric_limits<float>::min();
        constexpr float MaxF = std::numeric_limits<float>::max();
        sf::FloatRect bounds({MaxF, MaxF}, {MinF, MinF});

        for (unsigned int i = 0; i < component().getSize(); ++i) {
            const auto& v     = component()[i];
            bounds.position.x = std::min(v.pos.x, bounds.position.x);
            bounds.position.y = std::min(v.pos.y, bounds.position.y);
            bounds.size.x     = std::max(v.pos.x, bounds.size.x);
            bounds.size.y     = std::max(v.pos.y, bounds.size.y);
        }

        getTransform().setScale((bounds.size.x - bounds.position.x) / size.x,
                                (bounds.size.y - bounds.position.y) / size.y);
    }
}

} // namespace gfx
} // namespace bl
