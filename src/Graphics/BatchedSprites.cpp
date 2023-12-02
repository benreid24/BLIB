#include <BLIB/Graphics/BatchedSprites.hpp>

namespace bl
{
namespace gfx
{
BatchedSprites::BatchedSprites(engine::Engine& engine, rc::res::TextureRef texture,
                               unsigned int initialCapacity) {
    create(engine, texture, initialCapacity);
}

void BatchedSprites::create(engine::Engine& engine, rc::res::TextureRef texture,
                            unsigned int initialCapacity) {
    Drawable::create(engine, engine.renderer(), texture, initialCapacity);
    Textured::create(engine.ecs(), entity(), texture);
    Transform2D::create(engine.ecs(), entity());
}

void BatchedSprites::scaleToSize(const glm::vec2&) {
    BL_LOG_ERROR << "Ignoring request to scale SpriteBatch to size";
}

} // namespace gfx
} // namespace bl
