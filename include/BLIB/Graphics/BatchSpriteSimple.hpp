#ifndef BLIB_GRAPHICS_BATCHSPRITESIMPLE_HPP
#define BLIB_GRAPHICS_BATCHSPRITESIMPLE_HPP

#include <BLIB/Graphics/BatchedSprites.hpp>
#include <span>

namespace bl
{
namespace gfx
{
/**
 * @brief Simpler version of BatchSprite that requires manual population of the vertices
 *
 * @ingroup Graphics
 */
class BatchSpriteSimple {
public:
    /**
     * @brief Does nothing
     */
    BatchSpriteSimple();

    /**
     * @brief Creates the simple batched sprite and populates
     *
     * @param batch The batch to be a part of
     * @param textureSource The texture source rectangle to use
     */
    BatchSpriteSimple(BatchedSprites& batch, const sf::FloatRect& textureSource);

    /**
     * @brief Creates the simple batched sprite and populates. Allocates multiple sprites
     *
     * @param batch The batch to be a part of
     * @param textureSource The texture source rectangles to use
     */
    BatchSpriteSimple(BatchedSprites& batch, std::span<sf::FloatRect> textureSources);

    /**
     * @brief Releases its allocation from its owner and updates draw command
     */
    ~BatchSpriteSimple();

    /**
     * @brief Creates the simple batched sprite and populates the indices and vertices
     *
     * @param batch The batch to be a part of
     * @param textureSource The texture source rectangle to use
     */
    void create(BatchedSprites& batch, const sf::FloatRect& textureSource);

    /**
     * @brief Creates the simple batched sprite and populates. Allocates multiple sprites
     *
     * @param batch The batch to be a part of
     * @param textureSource The texture source rectangles to use
     */
    void create(BatchedSprites& batch, std::span<const sf::FloatRect> textureSources);

    /**
     * @brief Removes the sprite from the batch
     */
    void remove();

    /**
     * @brief Access the vertices for this sprite
     */
    std::span<rc::prim::Vertex> getVertices() {
        return {alloc.getVertices(), alloc.getInfo().vertexSize};
    }

    /**
     * @brief Access the indices for this sprite. Manual manipulation should not be required
     */
    std::span<std::uint32_t> getIndices() {
        return {alloc.getIndices(), alloc.getInfo().indexSize};
    }

    /**
     * @brief Calls commit on the owning batch
     */
    void commit();

private:
    BatchedSprites* owner;
    rc::buf::BatchIndexBuffer::AllocHandle alloc;
};

} // namespace gfx
} // namespace bl

#endif
