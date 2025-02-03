#ifndef BLIB_COMPONENTS_ANIMATION2D_HPP
#define BLIB_COMPONENTS_ANIMATION2D_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace sys
{
class Animation2DSystem;
}
namespace engine
{
class Engine;
}

namespace com
{
/**
 * @brief Component for non-slideshow 2d animations
 *
 * @ingroup Components
 */
struct Animation2D : public rc::rcom::DrawableBase {
    const Animation2DPlayer* player;

    /**
     * @brief Does nothing
     */
    Animation2D();

    /**
     * @brief Creates the index buffer to render the animation. Does not register ECS dependency.
     *        The player must remain valid while the animation exists
     *
     * @param engine The game engine instance
     * @param anim The animation to use when creating the vertices
     */
    void create(engine::Engine& engine, const Animation2DPlayer& anim);

    /**
     * @brief Returns the default material pipeline for rendering
     */
    virtual std::uint32_t getDefaultMaterialPipelineId() const override {
        return rc::Config::MaterialPipelineIds::Geometry2DSkinned;
    }

private:
    void* systemHandle;

    void initDrawParams(const rc::prim::DrawParameters& params);
    void updateDrawParams(std::uint32_t indexStart, std::uint32_t indexCount);

    friend class sys::Animation2DSystem;
};

} // namespace com
} // namespace bl

#endif
