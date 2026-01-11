#include <BLIB/Graphics/BatchedSlideshows.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
BatchedSlideshows::BatchedSlideshows(engine::World& world, unsigned int initialCapacity) {
    create(world, initialCapacity);
}

void BatchedSlideshows::create(engine::World& world, unsigned int initialCapacity) {
    Drawable::create(world, world.engine().renderer(), initialCapacity);
    Transform2D::create(world.engine().ecs(), entity());
}

void BatchedSlideshows::scaleToSize(const glm::vec2&) {
    BL_LOG_ERROR << "Ignoring request to scale BatchedSlideshows to size";
}

} // namespace gfx
} // namespace bl
