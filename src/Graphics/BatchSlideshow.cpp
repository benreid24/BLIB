#include <BLIB/Graphics/BatchSlideshow.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace gfx
{
BatchSlideshow::BatchSlideshow()
: engine(nullptr)
, owner(nullptr)
, playerEntity(ecs::InvalidEntity)
, dirty(false)
, autoCommit(true) {}

BatchSlideshow::BatchSlideshow(engine::Engine& engine, BatchedSlideshows& batch, ecs::Entity player)
: BatchSlideshow() {
    create(engine, batch, player);
}

BatchSlideshow::~BatchSlideshow() {
    removePlayerDep();
    remove();
}

void BatchSlideshow::create(engine::Engine& e, BatchedSlideshows& batch, ecs::Entity player) {
    engine = &e;
    if (!validatePlayer(player)) { return; }
    owner = &batch;
    setPlayer(player);
}

void BatchSlideshow::setPlayer(ecs::Entity ent) {
    removePlayerDep();
    playerEntity = ent;
    markDirty();
}

void BatchSlideshow::scaleToSize(const glm::vec2& size) {
    com::Animation2DPlayer* player = validatePlayer(playerEntity);
    if (!player) {
        BL_LOG_ERROR << "Cannot scale invalid animation to size";
        return;
    }

    const sf::Vector2f srcSize = player->getAnimation()->getMaxSize();
    transform.setScale({size.x / srcSize.x, size.y / srcSize.y});
}

com::Transform2D& BatchSlideshow::getLocalTransform() {
    markDirty();
    return transform;
}

void BatchSlideshow::remove() {
    if (alloc.isValid()) {
        if (alloc.release() && owner) {
            removePlayerDep();
            owner->component().updateDrawParams();
            owner = nullptr;
        }
    }
}

bool BatchSlideshow::isCreated() const { return owner != nullptr; }

void BatchSlideshow::markDirty() {
    dirty = true;
    if (autoCommit && owner && !updateHandle.isQueued()) {
        updateHandle = engine->systems().addFrameTask(engine::FrameStage::RendererDataSync,
                                                      std::bind(&BatchSlideshow::commit, this));
    }
}

void BatchSlideshow::commit() {
    if (dirty && owner) {
        com::Animation2DPlayer* player = validatePlayer(playerEntity);
        if (!player) {
            BL_LOG_ERROR << "Animation player invalidated";
            return;
        }
        if (player->getPlayerIndex() == com::Animation2DPlayer::InvalidIndex) {
            BL_LOG_ERROR << "Animation player was not initialized by system";
            return;
        }
        if (!player->getAnimation() || player->getAnimation()->frameCount() == 0) {
            BL_LOG_WARN << "Ignoring empty animation";
            return;
        }
        addPlayerDep();

        if (!alloc.isValid()) {
            alloc = owner->component().indexBuffer.allocate(4, 6);

            alloc.getIndices()[0] = 0 + alloc.getInfo().vertexStart;
            alloc.getIndices()[1] = 1 + alloc.getInfo().vertexStart;
            alloc.getIndices()[2] = 2 + alloc.getInfo().vertexStart;
            alloc.getIndices()[3] = 0 + alloc.getInfo().vertexStart;
            alloc.getIndices()[4] = 2 + alloc.getInfo().vertexStart;
            alloc.getIndices()[5] = 3 + alloc.getInfo().vertexStart;
        }

        rc::prim::SlideshowVertex* vertices = alloc.getVertices();
        const sf::FloatRect src(player->getAnimation()->getFrame(0).shards.front().source);
        vertices[0].pos = {0.f, 0.f, 0.f};
        vertices[1].pos = {src.size.x, 0.f, 0.f};
        vertices[2].pos = {src.size.x, src.size.y, 0.f};
        vertices[3].pos = {0.f, src.size.y, 0.f};
        for (unsigned int i = 0; i < 4; ++i) {
            rc::prim::SlideshowVertex& vertex = vertices[i];
            vertex.slideshowIndex             = player->getPlayerIndex();
            vertex.color                      = {1.f, 1.f, 1.f, 1.f};
            vertex.pos                        = transform.transformPoint(vertex.pos);
        }

        alloc.commit();
        owner->component().updateDrawParams();
    }
}

void BatchSlideshow::disableAutoCommit(bool d) { autoCommit = !d; }

com::Animation2DPlayer* BatchSlideshow::validatePlayer(ecs::Entity ent) {
    com::Animation2DPlayer* player = engine->ecs().getComponent<com::Animation2DPlayer>(ent);
    if (!player) {
        BL_LOG_ERROR << "Given player entity " << ent << " is missing Animation2DPlayer component";
        return nullptr;
    }
    if (!player->isForSlideshow()) {
        BL_LOG_ERROR << "Given animation player entity " << ent
                     << " is not configured for Slideshow animations";
        return nullptr;
    }
    return player;
}

void BatchSlideshow::addPlayerDep() {
    if (playerEntity != ecs::InvalidEntity && owner && owner->entity() != ecs::InvalidEntity) {
        engine->ecs().addDependency(playerEntity, owner->entity());
    }
}

void BatchSlideshow::removePlayerDep() {
    if (playerEntity != ecs::InvalidEntity && owner &&
        engine->ecs().entityExists(owner->entity())) {
        engine->ecs().removeDependencyAndDestroyIfPossible(playerEntity, owner->entity());
    }
}

} // namespace gfx
} // namespace bl
