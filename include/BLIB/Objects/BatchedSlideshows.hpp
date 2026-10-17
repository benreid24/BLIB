#ifndef BLIB_OBJECTS_BATCHEDSLIDESHOWS_HPP
#define BLIB_OBJECTS_BATCHEDSLIDESHOWS_HPP

#include <BLIB/Components/BatchedSlideshows.hpp>
#include <BLIB/Objects/Components/Transform2D.hpp>
#include <BLIB/Objects/Drawable.hpp>

namespace bl
{
namespace obj
{
/**
 * @brief Collection of batched slideshow animations that are all rendered together
 *
 * @ingroup Objects
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
     * @param world The world to create the object in
     * @param initialCapacity The number of slideshows to allocate for
     */
    BatchedSlideshows(engine::World& world, unsigned int initialCapacity = 16);

    /**
     * @brief Creates the batch slideshow entity
     *
     * @param world The world to create the object in
     * @param initialCapacity The number of slideshows to allocate for
     */
    void create(engine::World& world, unsigned int initialCapacity = 16);

private:
    virtual void scaleToSize(const glm::vec2&) override;
};

} // namespace obj
} // namespace bl

#endif
