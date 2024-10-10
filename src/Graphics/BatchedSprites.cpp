#include <BLIB/Graphics/BatchedSprites.hpp>

namespace bl
{
namespace gfx
{
BatchedSprites::BatchedSprites(engine::World& world, rc::res::TextureRef texture,
                               unsigned int initialCapacity) {
    create(world, texture, initialCapacity);
}

void BatchedSprites::create(engine::World& world, rc::res::TextureRef texture,
                            unsigned int initialCapacity) {
    Drawable::create(world, world.engine().renderer(), texture, initialCapacity);
    Textured::create(world.engine().ecs(), entity(), texture);
    Transform2D::create(world.engine().ecs(), entity());
}

void BatchedSprites::scaleToSize(const glm::vec2&) {
    BL_LOG_ERROR << "Ignoring request to scale SpriteBatch to size";
}

} // namespace gfx
} // namespace bl
