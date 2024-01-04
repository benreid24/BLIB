#ifndef BLIB_GRAPHICS_BATCHSPRITE_HPP
#define BLIB_GRAPHICS_BATCHSPRITE_HPP

#include <BLIB/Graphics/BatchedSprites.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief A single sprite in a batch of sprites
 *
 * @ingroup Graphics
 */
class BatchSprite {
public:
    /**
     * @brief Does nothing
     */
    BatchSprite();

    /**
     * @brief Creates the sprite in the given batch
     *
     * @param batch The batch of sprites to create within
     * @param textureSource The size in pixels. Sprite size is based on the texture size
     */
    BatchSprite(engine::Engine& engine, BatchedSprites& batch, const sf::FloatRect& textureSource);

    /**
     * @brief Creates the sprite in the given batch
     *
     * @param batch The batch of sprites to create within
     * @param textureSource The size in pixels. Sprite size is based on the texture size
     */
    void create(engine::Engine& engine, BatchedSprites& batch, const sf::FloatRect& textureSource);

    /**
     * @brief Changes the source texture coordinates. Must already be created
     *
     * @param textureSource The new texture coordinate source area
     */
    void updateSourceRect(const sf::FloatRect& textureSource);

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

    /**
     * @brief Disables auto-commit of the geometry when parameters change
     *
     * @param disable True to disable, false to auto commit. Default is false
     */
    void disableAutoCommit(bool disable);

    /**
     * @brief Manually commit the geometry to the batch. Only required if auto-commit is off
     */
    void commit();

private:
    engine::Engine* engine;
    BatchedSprites* owner;
    sf::FloatRect source;
    com::Transform2D transform;
    rc::buf::BatchIndexBuffer::AllocHandle alloc;
    bool dirty;
    bool autoCommit;
    engine::Systems::TaskHandle updateHandle;

    void markDirty();
};

} // namespace gfx
} // namespace bl

#endif
