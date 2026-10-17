#ifndef BLIB_OBJECTS_2D_DUMMY_HPP
#define BLIB_OBJECTS_2D_DUMMY_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/ECS/EntityBacked.hpp>
#include <BLIB/Objects/Components/OverlayScalable.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace obj
{
namespace o2d
{
/**
 * @brief Dummy graphical entity that is not actually drawn. Useful for anchoring in parent trees
 *
 * @ingroup Objects
 */
class Dummy
: public ecs::EntityBacked
, public bcom::OverlayScalable {
public:
    /**
     * @brief Does nothing
     */
    Dummy() = default;

    /**
     * @brief Copies from the given dummy
     *
     * @param move The dummy to take over
     */
    Dummy(Dummy&& move) = default;

    /**
     * @brief Destroys the underlying entity
     */
    ~Dummy() = default;

    /**
     * @brief Creates the dummy entity with Transform2D and OverlayScaler components
     *
     * @param world The world to create the object in
     */
    void create(engine::World& world);

    /**
     * @brief Sets the size of the dummy object for overlay scaling
     *
     * @param size The size in overlay units
     */
    void setSize(const glm::vec2& size);

private:
    virtual void ensureLocalSizeUpdated() override {}
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
