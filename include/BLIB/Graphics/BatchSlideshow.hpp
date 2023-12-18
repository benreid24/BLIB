#ifndef BLIB_GRAPHICS_BATCHSLIDESHOW_HPP
#define BLIB_GRAPHICS_BATCHSLIDESHOW_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/Graphics/BatchedSlideshows.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief A single slideshow that is part of a batch of slideshows
 *
 * @ingroup Graphics
 */
class BatchSlideshow {
public:
    /**
     * @brief Does nothing
     */
    BatchSlideshow();

    /**
     * @brief Creates the batched slideshow
     *
     * @param engine The game engine instance
     * @param batch The batch to be a part of
     * @param playerEntity The ECS entity with the Animation2DPlayer component to use
     */
    BatchSlideshow(engine::Engine& engine, BatchedSlideshows& batch, ecs::Entity playerEntity);

    /**
     * @brief Cleans up
     */
    ~BatchSlideshow();

    /**
     * @brief Creates the batched slideshow
     *
     * @param engine The game engine instance
     * @param batch The batch to be a part of
     * @param playerEntity The ECS entity with the Animation2DPlayer component to use
     */
    void create(engine::Engine& engine, BatchedSlideshows& batch, ecs::Entity playerEntity);

    /**
     * @brief Changes the player of an already created batch slideshow
     *
     * @param playerEntity The ECS entity with the Animation2DPlayer component to use
     */
    void setPlayer(ecs::Entity playerEntity);

    /**
     * @brief Returns the player entity for this animation
     */
    com::Animation2DPlayer& getPlayer();

    /**
     * @brief Helper method to scale the sprite to the given size in batch coordinates
     *
     * @param size The size to scale to
     */
    void scaleToSize(const glm::vec2& size);

    /**
     * @brief Returns the local transform of the sprite. Determines the positions of the vertices
     *        within the batch
     */
    const com::Transform2D& getLocalTransform() const { return transform; }

    /**
     * @brief Returns the local transform of the sprite. Determines the positions of the vertices
     *        within the batch. This override assumes the transform is modified and marks dirty
     */
    com::Transform2D& getLocalTransform();

    /**
     * @brief Removes the sprite from the batch
     */
    void remove();

    /**
     * @brief Returns whether or not the sprite has been created
     */
    bool isCreated() const;

private:
    engine::Engine* engine;
    BatchedSlideshows* owner;
    ecs::Entity playerEntity;
    com::Transform2D transform;
    rc::buf::BatchIndexBufferT<rc::prim::SlideshowVertex>::AllocHandle alloc;
    bool dirty;
    engine::Systems::TaskHandle updateHandle;

    com::Animation2DPlayer* validatePlayer(ecs::Entity ent);
    void addPlayerDep();
    void removePlayerDep();

    void markDirty();
    void commit();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::Animation2DPlayer& BatchSlideshow::getPlayer() {
    return *engine->ecs().getComponent<com::Animation2DPlayer>(playerEntity);
}

} // namespace gfx
} // namespace bl

#endif
