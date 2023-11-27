#ifndef BLIB_GRAPHICS_DUMMY2D_HPP
#define BLIB_GRAPHICS_DUMMY2D_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Graphics/Components/EntityBacked.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>

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
: public bcom::EntityBacked
, public bcom::OverlayScalable {
public:
    /**
     * @brief Does nothing
     */
    Dummy2D();

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
     * @param engine The game engine instance
     */
    void create(engine::Engine& engine);

    /**
     * @brief Sets the size of the dummy object for overlay scaling
     *
     * @param size The size in overlay units
     */
    void setSize(const glm::vec2& size);

    /**
     * @brief Controls whether or not children of the dummy are rendered
     *
     * @param hidden True to hide, false to render
     */
    void setHidden(bool hidden);

private:
    bool hidden;
    rc::ovy::OverlayObject* object;

    virtual void ensureLocalSizeUpdated() override {}
};

} // namespace gfx
} // namespace bl

#endif
