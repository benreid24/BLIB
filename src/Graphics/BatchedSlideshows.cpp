#include <BLIB/Graphics/BatchedSlideshows.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
BatchedSlideshows::BatchedSlideshows(engine::Engine& engine, unsigned int initialCapacity) {
    create(engine, initialCapacity);
}

void BatchedSlideshows::create(engine::Engine& engine, unsigned int initialCapacity) {
    Drawable::create(engine, engine.renderer().vulkanState(), initialCapacity);
    Transform2D::create(engine.ecs(), entity());
}

void BatchedSlideshows::scaleToSize(const glm::vec2&) {
    BL_LOG_ERROR << "Ignoring request to scale BatchedSlideshows to size";
}

} // namespace gfx
} // namespace bl
