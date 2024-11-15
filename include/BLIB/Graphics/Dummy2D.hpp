#ifndef BLIB_GRAPHICS_DUMMY2D_HPP
#define BLIB_GRAPHICS_DUMMY2D_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/ECS/EntityBacked.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace gfx
{
/**
 * @brief Dummy graphical entity that is not actually drawn. Useful for anchoring in parent trees
 *
 * @ingroup Graphics
 */
class Dummy2D
: public ecs::EntityBacked
, public bcom::OverlayScalable {
public:
    /**
     * @brief Does nothing
     */
    Dummy2D() = default;

    /**
     * @brief Copies from the given dummy
     *
     * @param move The dummy to take over
     */
    Dummy2D(Dummy2D&& move) = default;

    /**
     * @brief Destroys the underlying entity
     */
    ~Dummy2D() = default;

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

} // namespace gfx
} // namespace bl

#endif
