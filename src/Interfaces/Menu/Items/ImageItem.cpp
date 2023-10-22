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

com::Transform2D& ImageItem::doCreate(engine::Engine& engine, ecs::Entity parent) {
    sprite.create(engine, texture);
    sprite.setParent(parent);
    return sprite.getTransform();
}

void ImageItem::doSceneAdd(rc::Overlay* overlay) {
    sprite.addToScene(overlay, rc::UpdateSpeed::Static);
}

void ImageItem::doSceneRemove() { sprite.removeFromScene(); }

ecs::Entity ImageItem::getEntity() const { return sprite.entity(); }

} // namespace menu
} // namespace bl
