#ifndef BLIB_GRAPHICS_BATCHEDSPRITES_HPP
#define BLIB_GRAPHICS_BATCHEDSPRITES_HPP

#include <BLIB/Components/BatchedSprites.hpp>
#include <BLIB/Graphics/Components/Textured.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Set of sprites batched into one entity
 *
 * @ingroup Graphics
 */
class BatchedSprites
: public Drawable<com::BatchedSprites>
, public bcom::Transform2D
, public bcom::Textured {
public:
    /**
     * @brief Does nothing
     */
    BatchedSprites() = default;

    /**
     * @brief Creates the sprite batch
     *
     * @param engine The game engine instance
     * @param texture The texture to use
     * @param initialCapacity The estimated number of sprites that will be batched
     */
    BatchedSprites(engine::Engine& engine, rc::res::TextureRef texture,
                   unsigned int initialCapacity = 16);

    /**
     * @brief Creates the sprite batch
     *
     * @param engine The game engine instance
     * @param texture The texture to use
     * @param initialCapacity The estimated number of sprites that will be batched
     */
    void create(engine::Engine& engine, rc::res::TextureRef texture,
                unsigned int initialCapacity = 16);

private:
    virtual void scaleToSize(const glm::vec2&) override;
};

} // namespace gfx
} // namespace bl

#endif
