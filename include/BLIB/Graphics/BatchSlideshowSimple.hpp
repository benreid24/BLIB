#ifndef BLIB_GRAPHICS_BATCHSLIDESHOWSIMPLE_HPP
#define BLIB_GRAPHICS_BATCHSLIDESHOWSIMPLE_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/Graphics/BatchedSlideshows.hpp>
#include <span>

namespace bl
{
namespace gfx
{
/**
 * @brief Thin wrapper for index buffer allocations in BatchedSlideshows
 *
 * @ingroup Graphics
 */
class BatchSlideshowSimple {
public:
    /**
     * @brief Does nothing
     */
    BatchSlideshowSimple();

    /**
     * @brief Creates the slideshow batch and populates the indices and player fields
     *
     * @param engine The game engine instance
     * @param batch The batch to allocate from
     * @param playerEntity The ECS entity with the player component to use
     */
    BatchSlideshowSimple(engine::Engine& engine, BatchedSlideshows& batch,
                         ecs::Entity playerEntity);

    /**
     * @brief Creates the slideshow batch and populates the indices and player fields
     *
     * @param engine The game engine instance
     * @param batch The batch to allocate from
     * @param playerEntity The ECS entity with the player component to use
     */
    void create(engine::Engine& engine, BatchedSlideshows& batch, ecs::Entity playerEntity);

    /**
     * @brief Updates the player component. Does some basic validation
     *
     * @param engine The game engine instance
     * @param playerEntity The ECS entity with the player component to use
     */
    void setPlayer(engine::Engine& engine, ecs::Entity playerEntity);

    /**
     * @brief Access the player for this animation
     */
    com::Animation2DPlayer& getPlayer() { return *player; }

    /**
     * @brief Access the vertices for this animation
     */
    std::span<rc::prim::SlideshowVertex> getVertices() { return {alloc.getVertices(), 4}; }

    /**
     * @brief Access the indices for this animation. Manual manipulation should not be required
     */
    std::span<std::uint32_t> getIndices() { return {alloc.getIndices(), 6}; }

    /**
     * @brief Calls commit on the owning batch
     */
    void commit();

private:
    BatchedSlideshows* owner;
    com::Animation2DPlayer* player;
    rc::buf::BatchIndexBufferT<rc::prim::SlideshowVertex>::AllocHandle alloc;

    com::Animation2DPlayer* validatePlayer(engine::Engine& engine, ecs::Entity ent);
};

} // namespace gfx
} // namespace bl

#endif
