#include <BLIB/Components/Sprite.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace com
{
Sprite::Sprite(rc::Renderer& renderer, const rc::res::TextureRef& texture,
               const sf::FloatRect& region) {
    create(&renderer, texture, region);
}

void Sprite::create(rc::Renderer* renderer, const rc::res::TextureRef& txtr, sf::FloatRect region) {
    texture = txtr;
    if (region.width == 0.f || region.height == 0.f) {
        region.left   = 0.f;
        region.height = 0.f;
        region.width  = texture->size().x;
        region.height = texture->size().y;
    }
    size.x = region.width;
    size.y = region.height;

    if (buffer.vertexCount() == 0) {
#ifdef BLIB_DEBUG
        if (!renderer) {
            throw std::runtime_error("Renderer must be supplied to first call to create()");
        }
#endif
        buffer.create(renderer->vulkanState(), 4, 6);
        buffer.indices() = {0, 1, 2, 0, 2, 3};
    }

    auto& vertices = buffer.vertices();
    setTextureSource(region);

    vertices[0].pos   = {0.f, 0.f, 0.f};
    vertices[0].color = {1.f, 1.f, 1.f, 1.f};

    vertices[1].pos   = {region.width, 0.f, 0.f};
    vertices[1].color = {1.f, 1.f, 1.f, 1.f};

    vertices[2].pos   = {region.width, region.height, 0.f};
    vertices[2].color = {1.f, 1.f, 1.f, 1.f};

    vertices[3].pos   = {0.f, region.height, 0.f};
    vertices[3].color = {1.f, 1.f, 1.f, 1.f};

    for (auto& v : vertices) { v.texCoord = texture->convertCoord(v.texCoord); }

    refreshTrans();
    drawParams = buffer.getDrawParameters();
    buffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);
}

void Sprite::setTextureSource(const sf::FloatRect& region) {
    auto& vertices      = buffer.vertices();
    const float leftX   = region.left / texture->size().x;
    const float topY    = region.top / texture->size().y;
    const float rightX  = (region.left + region.width) / texture->size().x;
    const float bottomY = (region.top + region.height) / texture->size().y;

    vertices[0].texCoord = {leftX, topY};
    vertices[1].texCoord = {rightX, topY};
    vertices[2].texCoord = {rightX, bottomY};
    vertices[3].texCoord = {leftX, bottomY};

    size.x = region.width;
    size.y = region.height;

    buffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);
}

void Sprite::setTexture(const rc::res::TextureRef& txtr, bool reset) {
    texture = txtr;
    refreshTrans();

    if (reset) {
        const sf::FloatRect region(0.f, 0.f, texture->size().x, texture->size().y);
        auto& vertices = buffer.vertices();

        vertices[0].pos   = {0.f, 0.f, 0.f};
        vertices[0].color = {1.f, 1.f, 1.f, 1.f};

        vertices[1].pos   = {region.width, 0.f, 0.f};
        vertices[1].color = {1.f, 1.f, 1.f, 1.f};

        vertices[2].pos   = {region.width, region.height, 0.f};
        vertices[2].color = {1.f, 1.f, 1.f, 1.f};

        vertices[3].pos   = {0.f, region.height, 0.f};
        vertices[3].color = {1.f, 1.f, 1.f, 1.f};

        setTextureSource(region);
    }
}

void Sprite::setColor(const glm::vec4& color) {
    for (auto& v : buffer.vertices()) { v.color = color; }
    if (buffer.vertexCount() > 0) {
        buffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);
    }
    refreshTrans();
}

void Sprite::refreshTrans() {
    const bool wasTrans  = containsTransparency;
    containsTransparency = texture->containsTransparency() || (buffer.vertices()[0].color.a > 0.f &&
                                                               buffer.vertices()[0].color.a < 1.f);
    if (containsTransparency != wasTrans) { rebucket(); }
}

} // namespace com
} // namespace bl
