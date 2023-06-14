#include <BLIB/Render/Components/Sprite.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace com
{
Sprite::Sprite(Renderer& renderer, const res::TextureRef& texture, const sf::FloatRect& region) {
    create(renderer, texture, region);
}

void Sprite::create(Renderer& renderer, const res::TextureRef& txtr, sf::FloatRect region) {
    texture = txtr;
    if (region.width == 0.f || region.height == 0.f) {
        region.left   = 0.f;
        region.height = 0.f;
        region.width  = texture->sizeF.x;
        region.height = texture->sizeF.y;
    }
    size.x = region.width;
    size.y = region.height;

    if (buffer.vertexCount() == 0) { buffer.create(renderer.vulkanState(), 4, 6); }

    prim::Vertex* vertices = renderer.vulkanState().transferEngine.createOneTimeVertexStorage(4);
    std::uint32_t* indices = renderer.vulkanState().transferEngine.createOneTimeIndexStorage(6);
    buffer.vertices().configureWrite(vertices, 0, 4 * sizeof(prim::Vertex));
    buffer.indices().configureWrite(indices, 0, 6 * sizeof(std::uint32_t));

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    const float leftX   = region.left / texture->sizeF.x;
    const float topY    = region.top / texture->sizeF.y;
    const float rightX  = (region.left + region.width) / texture->sizeF.x;
    const float bottomY = (region.top + region.height) / texture->sizeF.y;

    vertices[0].texCoord = {leftX, topY};
    vertices[0].pos      = {0.f, 0.f, 0.f};

    vertices[1].texCoord = {rightX, topY};
    vertices[1].pos      = {region.width, 0.f, 0.f};

    vertices[2].texCoord = {rightX, bottomY};
    vertices[2].pos      = {region.width, region.height, 0.f};

    vertices[3].texCoord = {leftX, bottomY};
    vertices[3].pos      = {0.f, region.height, 0.f};

    drawParams = buffer.getDrawParameters();
    buffer.sendToGPU();
}

void Sprite::setTexture(const res::TextureRef& txtr) { texture = txtr; }

} // namespace com
} // namespace render
} // namespace bl
