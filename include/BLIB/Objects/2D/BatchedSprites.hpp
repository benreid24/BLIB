#ifndef BLIB_OBJECTS_2D_BATCHEDSPRITES_HPP
#define BLIB_OBJECTS_2D_BATCHEDSPRITES_HPP

#include <BLIB/Components/BatchedSprites.hpp>
#include <BLIB/Objects/2D/DrawableObject2D.hpp>
#include <BLIB/Objects/Components/Textured.hpp>
#include <BLIB/Objects/Components/Transform2D.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Set of sprites batched into one entity
 *
 * @ingroup Objects
 */
class BatchedSprites
: public DrawableObject2D<com::BatchedSprites>
, public bcom::Textured {
public:
    /**
     * @brief Does nothing
     */
    BatchedSprites() = default;

    /**
     * @brief Creates the sprite batch
     *
     * @param world The world to create the object in
     * @param texture The texture to use
     * @param initialCapacity The estimated number of sprites that will be batched
     */
    BatchedSprites(engine::World& world, rc::res::TextureRef texture,
                   unsigned int initialCapacity = 16);

    /**
     * @brief Creates the sprite batch
     *
     * @param world The world to create the object in
     * @param texture The texture to use
     * @param initialCapacity The estimated number of sprites that will be batched
     */
    void create(engine::World& world, rc::res::TextureRef texture,
                unsigned int initialCapacity = 16);

private:
    virtual void scaleToSize(const glm::vec2&) override;
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
