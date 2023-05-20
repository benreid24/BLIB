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

    vertices[0].texCoord.x = region.left;
    vertices[0].texCoord.y = region.top;
    vertices[0].pos        = {vertices[0].texCoord, 0.f};

    vertices[1].texCoord.x = region.left + region.width;
    vertices[1].texCoord.y = region.top;
    vertices[1].pos        = {vertices[0].texCoord, 0.f};

    vertices[2].texCoord.x = region.left + region.width;
    vertices[2].texCoord.y = region.top + region.height;
    vertices[2].pos        = {vertices[0].texCoord, 0.f};

    vertices[3].texCoord.x = region.left;
    vertices[3].texCoord.y = region.top + region.height;
    vertices[3].pos        = {vertices[0].texCoord, 0.f};
}

void Sprite::setTexture(const res::TextureRef& txtr) { texture = txtr; }

} // namespace com
} // namespace render
} // namespace bl
