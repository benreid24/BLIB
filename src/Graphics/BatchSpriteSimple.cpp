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
    const float leftX          = source.left / owner->getTexture()->size().x;
    const float topY           = source.top / owner->getTexture()->size().y;
    const float rightX         = (source.left + source.width) / owner->getTexture()->size().x;
    const float bottomY        = (source.top + source.height) / owner->getTexture()->size().y;

    vertices[0].texCoord = owner->getTexture()->convertCoord({leftX, topY});
    vertices[0].pos      = {0.f, 0.f, 0.f};
    vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[1].texCoord = owner->getTexture()->convertCoord({rightX, topY});
    vertices[1].pos      = {source.width, 0.f, 0.f};
    vertices[1].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[2].texCoord = owner->getTexture()->convertCoord({rightX, bottomY});
    vertices[2].pos      = {source.width, source.height, 0.f};
    vertices[2].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[3].texCoord = owner->getTexture()->convertCoord({leftX, bottomY});
    vertices[3].pos      = {0.f, source.height, 0.f};
    vertices[3].color    = {1.f, 1.f, 1.f, 1.f};
}

void BatchSpriteSimple::commit() {
    alloc.commit();
    owner->component().updateDrawParams();
}

} // namespace gfx
} // namespace bl
