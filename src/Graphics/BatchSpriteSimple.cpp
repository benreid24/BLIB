#include <BLIB/Graphics/BatchSpriteSimple.hpp>

namespace bl
{
namespace gfx
{
BatchSpriteSimple::BatchSpriteSimple()
: owner(nullptr) {}

BatchSpriteSimple::BatchSpriteSimple(BatchedSprites& batch, const sf::FloatRect& src) {
    create(batch, src);
}

BatchSpriteSimple::BatchSpriteSimple(BatchedSprites& batch, std::span<sf::FloatRect> src) {
    create(batch, src);
}

BatchSpriteSimple::~BatchSpriteSimple() { remove(); }

void BatchSpriteSimple::create(BatchedSprites& batch, const sf::FloatRect& source) {
    owner = &batch;

    alloc = owner->component().getBuffer().allocate(4, 6);

    alloc.getIndices()[0] = 0 + alloc.getInfo().vertexStart;
    alloc.getIndices()[1] = 1 + alloc.getInfo().vertexStart;
    alloc.getIndices()[2] = 2 + alloc.getInfo().vertexStart;
    alloc.getIndices()[3] = 0 + alloc.getInfo().vertexStart;
    alloc.getIndices()[4] = 2 + alloc.getInfo().vertexStart;
    alloc.getIndices()[5] = 3 + alloc.getInfo().vertexStart;

    rc::prim::Vertex* vertices = alloc.getVertices();
    const float leftX          = source.position.x / owner->getTexture()->size().x;
    const float topY           = source.position.y / owner->getTexture()->size().y;
    const float rightX  = (source.position.x + source.size.x) / owner->getTexture()->size().x;
    const float bottomY = (source.position.y + source.size.y) / owner->getTexture()->size().y;

    vertices[0].texCoord = owner->getTexture()->convertCoord({leftX, topY});
    vertices[0].pos      = {0.f, 0.f, 0.f};
    vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[1].texCoord = owner->getTexture()->convertCoord({rightX, topY});
    vertices[1].pos      = {source.size.x, 0.f, 0.f};
    vertices[1].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[2].texCoord = owner->getTexture()->convertCoord({rightX, bottomY});
    vertices[2].pos      = {source.size.x, source.size.y, 0.f};
    vertices[2].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[3].texCoord = owner->getTexture()->convertCoord({leftX, bottomY});
    vertices[3].pos      = {0.f, source.size.y, 0.f};
    vertices[3].color    = {1.f, 1.f, 1.f, 1.f};
}

void BatchSpriteSimple::create(BatchedSprites& batch, std::span<const sf::FloatRect> src) {
    owner = &batch;

    alloc = owner->component().getBuffer().allocate(4 * src.size(), 6 * src.size());

    for (unsigned int i = 0; i < src.size(); ++i) {
        const std::uint32_t indexOffset = alloc.getInfo().vertexStart + i * 4;
        const std::size_t ii            = i * 6;
        alloc.getIndices()[0 + ii]      = 0 + indexOffset;
        alloc.getIndices()[1 + ii]      = 1 + indexOffset;
        alloc.getIndices()[2 + ii]      = 2 + indexOffset;
        alloc.getIndices()[3 + ii]      = 0 + indexOffset;
        alloc.getIndices()[4 + ii]      = 2 + indexOffset;
        alloc.getIndices()[5 + ii]      = 3 + indexOffset;

        const sf::FloatRect& source = src[i];
        rc::prim::Vertex* vertices  = &alloc.getVertices()[i * 4];
        const float leftX           = source.position.x / owner->getTexture()->size().x;
        const float topY            = source.position.y / owner->getTexture()->size().y;
        const float rightX  = (source.position.x + source.size.x) / owner->getTexture()->size().x;
        const float bottomY = (source.position.y + source.size.y) / owner->getTexture()->size().y;

        vertices[0].texCoord = owner->getTexture()->convertCoord({leftX, topY});
        vertices[0].pos      = {0.f, 0.f, 0.f};
        vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[1].texCoord = owner->getTexture()->convertCoord({rightX, topY});
        vertices[1].pos      = {source.size.x, 0.f, 0.f};
        vertices[1].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[2].texCoord = owner->getTexture()->convertCoord({rightX, bottomY});
        vertices[2].pos      = {source.size.x, source.size.y, 0.f};
        vertices[2].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[3].texCoord = owner->getTexture()->convertCoord({leftX, bottomY});
        vertices[3].pos      = {0.f, source.size.y, 0.f};
        vertices[3].color    = {1.f, 1.f, 1.f, 1.f};
    }
}

void BatchSpriteSimple::commit() {
    alloc.commit();
    owner->component().updateDrawParams();
}

void BatchSpriteSimple::remove() {
    if (alloc.isValid()) {
        if (alloc.release() && owner) {
            owner->component().updateDrawParams();
            owner = nullptr;
        }
    }
}

} // namespace gfx
} // namespace bl
