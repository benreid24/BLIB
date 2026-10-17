#include <BLIB/Objects/2D/BatchedSprites.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
BatchedSprites::BatchedSprites(engine::World& world, rc::res::TextureRef texture,
                               unsigned int initialCapacity) {
    create(world, texture, initialCapacity);
}

void BatchedSprites::create(engine::World& world, rc::res::TextureRef texture,
                            unsigned int initialCapacity) {
    DrawableObject2D::create(world, world.engine().renderer(), texture, initialCapacity);
    Textured::create(world.engine().renderer(), &material(), texture);
}

void BatchedSprites::scaleToSize(const glm::vec2&) {
    BL_LOG_ERROR << "Ignoring request to scale SpriteBatch to size";
}

} // namespace o2d
} // namespace obj
} // namespace bl
