#include <BLIB/Render/Components/Sprite.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace com
{
namespace
{
constexpr std::array<std::uint32_t, 6> Indices = {0, 1, 2, 0, 2, 3};
}

Sprite::Sprite(Renderer& renderer, const res::TextureRef& texture, const sf::FloatRect& region) {
    create(renderer, texture, region);
}

void Sprite::create(Renderer& renderer, const res::TextureRef& txtr, sf::FloatRect region) {
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
        buffer.create(renderer.vulkanState(), 4, 6);
        buffer.vertices().configureWrite(vertices.data(), 0, 4 * sizeof(prim::Vertex));
        buffer.indices().configureWrite(Indices.data(), 0, 6 * sizeof(std::uint32_t));
    }

    const float leftX   = region.left / texture->size().x;
    const float topY    = region.top / texture->size().y;
    const float rightX  = (region.left + region.width) / texture->size().x;
    const float bottomY = (region.top + region.height) / texture->size().y;

    vertices[0].texCoord = {leftX, topY};
    vertices[0].pos      = {0.f, 0.f, 0.f};
    vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[1].texCoord = {rightX, topY};
    vertices[1].pos      = {region.width, 0.f, 0.f};
    vertices[1].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[2].texCoord = {rightX, bottomY};
    vertices[2].pos      = {region.width, region.height, 0.f};
    vertices[2].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[3].texCoord = {leftX, bottomY};
    vertices[3].pos      = {0.f, region.height, 0.f};
    vertices[3].color    = {1.f, 1.f, 1.f, 1.f};

    refreshTrans();
    drawParams = buffer.getDrawParameters();
    buffer.sendToGPU();
}

void Sprite::setTexture(const res::TextureRef& txtr) {
    texture = txtr;
    refreshTrans();
}

void Sprite::setColor(const glm::vec4& color) {
    for (auto& v : vertices) { v.color = color; }
    if (buffer.vertexCount() > 0) { buffer.sendToGPU(); }
    refreshTrans();
}

void Sprite::refreshTrans() {
    const bool wasTrans = containsTransparency;
    containsTransparency =
        texture->containsTransparency() || (vertices[0].color.a > 0.f && vertices[0].color.a < 1.f);
    if (containsTransparency != wasTrans) { rebucket(); }
}

} // namespace com
} // namespace gfx
} // namespace bl
