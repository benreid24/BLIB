#include <BLIB/Graphics/BatchSprite.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
BatchSprite::BatchSprite()
: engine(nullptr)
, owner(nullptr)
, dirty(false) {}

BatchSprite::BatchSprite(engine::Engine& engine, BatchedSprites& batch,
                         const sf::FloatRect& textureSource) {
    create(engine, batch, textureSource);
}

void BatchSprite::create(engine::Engine& e, BatchedSprites& batch,
                         const sf::FloatRect& textureSource) {
    engine = &e;
    owner  = &batch;
    source = textureSource;
    markDirty();
}

void BatchSprite::updateSourceRect(const sf::FloatRect& textureSource) {
    source = textureSource;
    markDirty();
}

void BatchSprite::scaleToSize(const glm::vec2& size) {
    getLocalTransform().setScale({size.x / source.width, size.y / source.height});
}

com::Transform2D& BatchSprite::getLocalTransform() {
    markDirty();
    return transform;
}

void BatchSprite::remove() {
    if (alloc.isValid()) {
        if (alloc.release() && owner) {
            owner->component().updateDrawParams();
            owner = nullptr;
        }
    }
}

bool BatchSprite::isCreated() const { return owner != nullptr; }

void BatchSprite::markDirty() {
    dirty = true;
    if (owner && !updateHandle.isQueued()) {
        updateHandle = engine->systems().addFrameTask(engine::FrameStage::RenderObjectInsertion,
                                                      std::bind(&BatchSprite::commit, this));
    }
}

void BatchSprite::commit() {
    if (dirty && owner) {
        dirty = false;

        if (!alloc.isValid()) {
            alloc = owner->component().getBuffer().allocate(4, 6);

            alloc.getIndices()[0] = 0 + alloc.getInfo().vertexStart;
            alloc.getIndices()[1] = 1 + alloc.getInfo().vertexStart;
            alloc.getIndices()[2] = 2 + alloc.getInfo().vertexStart;
            alloc.getIndices()[3] = 0 + alloc.getInfo().vertexStart;
            alloc.getIndices()[4] = 2 + alloc.getInfo().vertexStart;
            alloc.getIndices()[5] = 3 + alloc.getInfo().vertexStart;
        }

        rc::prim::Vertex* vertices = alloc.getVertices();
        const float leftX          = source.left / owner->getTexture()->size().x;
        const float topY           = source.top / owner->getTexture()->size().y;
        const float rightX         = (source.left + source.width) / owner->getTexture()->size().x;
        const float bottomY        = (source.top + source.height) / owner->getTexture()->size().y;

        vertices[0].texCoord = {leftX, topY};
        vertices[0].pos      = {0.f, 0.f, 0.f};
        vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[1].texCoord = {rightX, topY};
        vertices[1].pos      = {source.width, 0.f, 0.f};
        vertices[1].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[2].texCoord = {rightX, bottomY};
        vertices[2].pos      = {source.width, source.height, 0.f};
        vertices[2].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[3].texCoord = {leftX, bottomY};
        vertices[3].pos      = {0.f, source.height, 0.f};
        vertices[3].color    = {1.f, 1.f, 1.f, 1.f};

        for (unsigned int i = 0; i < 4; ++i) {
            vertices[i].pos = transform.transformPoint(vertices[i].pos);
        }

        alloc.commit();
        owner->component().updateDrawParams();
    }
}

} // namespace gfx
} // namespace bl
