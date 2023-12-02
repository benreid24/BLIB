#ifndef BLIB_GRAPHICS_BATCHEDSLIDESHOWS_HPP
#define BLIB_GRAPHICS_BATCHEDSLIDESHOWS_HPP

#include <BLIB/Components/BatchedSlideshows.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Collection of batched slideshow animations that are all rendered together
 *
 * @ingroup Graphics
 */
class BatchedSlideshows
: public Drawable<com::BatchedSlideshows>
, public bcom::Transform2D {
public:
    /**
     * @brief Does nothing
     */
    BatchedSlideshows() = default;

    /**
     * @brief Creates the batch slideshow entity
     *
     * @param engine The game engine instance
     * @param initialCapacity The number of slideshows to allocate for
     */
    BatchedSlideshows(engine::Engine& engine, unsigned int initialCapacity = 16);

    /**
     * @brief Creates the batch slideshow entity
     *
     * @param engine The game engine instance
     * @param initialCapacity The number of slideshows to allocate for
     */
    void create(engine::Engine& engine, unsigned int initialCapacity = 16);

private:
    virtual void scaleToSize(const glm::vec2&) override;
};

} // namespace gfx
} // namespace bl

#endif
