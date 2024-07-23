#include <BLIB/Graphics/BatchSlideshowSimple.hpp>

#include <BLIB/Engine.hpp>

namespace bl
{
namespace gfx
{
BatchSlideshowSimple::BatchSlideshowSimple()
: owner(nullptr) {}

BatchSlideshowSimple::BatchSlideshowSimple(engine::Engine& engine, BatchedSlideshows& batch,
                                           ecs::Entity playerEntity) {
    create(engine, batch, playerEntity);
}

void BatchSlideshowSimple::create(engine::Engine& engine, BatchedSlideshows& batch,
                                  ecs::Entity playerEntity) {
    owner = &batch;

    player                    = validatePlayer(engine, playerEntity);
    std::uint32_t playerIndex = 0;
    if (!player) { BL_LOG_ERROR << "Invalid animation player used, defaulting to player 0"; }
    else { playerIndex = player->getPlayerIndex(); }

    alloc = owner->component().indexBuffer.allocate(4, 6);

    alloc.getIndices()[0] = 0 + alloc.getInfo().vertexStart;
    alloc.getIndices()[1] = 1 + alloc.getInfo().vertexStart;
    alloc.getIndices()[2] = 2 + alloc.getInfo().vertexStart;
    alloc.getIndices()[3] = 0 + alloc.getInfo().vertexStart;
    alloc.getIndices()[4] = 2 + alloc.getInfo().vertexStart;
    alloc.getIndices()[5] = 3 + alloc.getInfo().vertexStart;

    rc::prim::SlideshowVertex* vertices = alloc.getVertices();
    const sf::FloatRect src(player->getAnimation()->getFrame(0).shards.front().source);
    vertices[0].pos = {0.f, 0.f, 0.f};
    vertices[1].pos = {src.width, 0.f, 0.f};
    vertices[2].pos = {src.width, src.height, 0.f};
    vertices[3].pos = {0.f, src.height, 0.f};
    for (unsigned int i = 0; i < 4; ++i) {
        rc::prim::SlideshowVertex& vertex = vertices[i];
        vertex.slideshowIndex             = playerIndex;
        vertex.color                      = {1.f, 1.f, 1.f, 1.f};
    }
}

void BatchSlideshowSimple::setPlayer(engine::Engine& engine, ecs::Entity pent) {
    player = validatePlayer(engine, pent);
    if (!player) {
        BL_LOG_ERROR << "Invalid animation player used, refusing to update player";
        return;
    }

    rc::prim::SlideshowVertex* vertices = alloc.getVertices();
    for (unsigned int i = 0; i < 4; ++i) {
        rc::prim::SlideshowVertex& vertex = vertices[i];
        vertex.slideshowIndex             = player->getPlayerIndex();
        vertex.color                      = {1.f, 1.f, 1.f, 1.f};
    }
}

void BatchSlideshowSimple::commit() {
    alloc.commit();
    owner->component().updateDrawParams();
}

com::Animation2DPlayer* BatchSlideshowSimple::validatePlayer(engine::Engine& engine,
                                                             ecs::Entity ent) {
    com::Animation2DPlayer* player = engine.ecs().getComponent<com::Animation2DPlayer>(ent);
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

} // namespace gfx
} // namespace bl
