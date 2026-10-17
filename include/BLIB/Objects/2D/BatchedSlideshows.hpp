#ifndef BLIB_OBJECTS_2D_BATCHEDSLIDESHOWS_HPP
#define BLIB_OBJECTS_2D_BATCHEDSLIDESHOWS_HPP

#include <BLIB/Components/BatchedSlideshows.hpp>
#include <BLIB/Objects/2D/DrawableObject2D.hpp>
#include <BLIB/Objects/Components/Transform2D.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Collection of batched slideshow animations that are all rendered together
 *
 * @ingroup Objects
 */
class BatchedSlideshows : public DrawableObject2D<com::BatchedSlideshows> {
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

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
