#include <BLIB/Objects/2D/BatchedSlideshows.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
BatchedSlideshows::BatchedSlideshows(engine::World& world, unsigned int initialCapacity) {
    create(world, initialCapacity);
}

void BatchedSlideshows::create(engine::World& world, unsigned int initialCapacity) {
    DrawableObject2D::create(world, world.engine().renderer(), initialCapacity);
}

void BatchedSlideshows::scaleToSize(const glm::vec2&) {
    BL_LOG_ERROR << "Ignoring request to scale BatchedSlideshows to size";
}

} // namespace o2d
} // namespace obj
} // namespace bl
