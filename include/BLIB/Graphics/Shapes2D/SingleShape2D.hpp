#ifndef BLIB_GRAPHICS_SHAPES_SINGLESHAPE2D_HPP
#define BLIB_GRAPHICS_SHAPES_SINGLESHAPE2D_HPP

#include <BLIB/Components/Shape2D.hpp>
#include <BLIB/Engine/Systems.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>
#include <BLIB/Graphics/Shapes2D/Shape2D.hpp>

namespace bl
{
namespace gfx
{
namespace s2d
{
/**
 * @brief Base class for individual 2d shapes. Provides index buffer creation and management
 *
 * @ingroup Graphics
 */
class SingleShape2D
: public Shape2D
, public Drawable<com::Shape2D>
, public bcom::OverlayScalable {
public:
    /**
     * @brief Destroys the shape
     */
    virtual ~SingleShape2D();

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;

protected:
    /**
     * @brief Initializes the shapes fields to sane defaults
     */
    SingleShape2D();

    /**
     * @brief Creates the ECS entity and required components
     *
     * @param engine The game engine instance
     */
    void create(engine::Engine& engine);

    /**
     * @brief Called when the local size is queried
     */
    virtual void ensureLocalSizeUpdated() override;

    /**
     * @brief Call whenever the shape geometry needs to be updated
     */
    virtual void notifyDirty() override;

private:
    bool dirty;
    engine::Systems::TaskHandle updateHandle;

    virtual void ensureUpdated() override;
};

} // namespace s2d
} // namespace gfx
} // namespace bl

#endif
