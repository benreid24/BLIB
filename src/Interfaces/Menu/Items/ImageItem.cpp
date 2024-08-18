#include <BLIB/Interfaces/Menu/Items/ImageItem.hpp>

namespace bl
{
namespace menu
{
ImageItem::Ptr ImageItem::create(const rc::res::TextureRef& texture) {
    return Ptr(new ImageItem(texture));
}

ImageItem::ImageItem(const rc::res::TextureRef& texture)
: texture(texture) {}

gfx::Sprite& ImageItem::getSprite() { return sprite; }

void ImageItem::setTexture(const rc::res::TextureRef& t) {
    texture = t;
    if (sprite.entity() != ecs::InvalidEntity) { sprite.setTexture(texture); }
}

glm::vec2 ImageItem::getSize() const {
    if (sprite.entity() != ecs::InvalidEntity) {
        const glm::vec2& size  = sprite.getLocalSize();
        const glm::vec2& scale = sprite.getTransform().getScale();
        return size * scale;
    }
    return texture->size();
}

void ImageItem::doCreate(engine::Engine& engine) { sprite.create(engine, texture); }

void ImageItem::doSceneAdd(rc::Scene* s) { sprite.addToScene(s, rc::UpdateSpeed::Static); }

void ImageItem::doSceneRemove() { sprite.removeFromScene(); }

ecs::Entity ImageItem::getEntity() const { return sprite.entity(); }

void ImageItem::draw(rc::scene::CodeScene::RenderContext& ctx) { sprite.draw(ctx); }

} // namespace menu
} // namespace bl
