#ifndef BLIB_PARTICLES_ECSCOMPONENT_HPP
#define BLIB_PARTICLES_ECSCOMPONENT_HPP

#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace pcl
{
/**
 * @brief Default drawable component for particle systems. Used by the default Renderer<T>
 *        implementation. Provides a vertex buffer with a single point located at (0, 0, 0)
 *        for each particle. Transparency and pipeline are provided by Renderer<T> via
 *        RenderConfig<T> which must be provided by the user
 *
 * @ingroup Particles
 */
struct EcsComponent : public bl::rc::rcom::DrawableBase {
    rc::buf::VertexBuffer vertexBuffer;

    /**
     * @brief Creates the component and vertex buffer
     *
     * @param engine The game engine instance
     * @param transparency Whether particles are semi-transparent or not
     * @param pipeline The pipeline to render with
     */
    EcsComponent(engine::Engine& engine, bool transparency, std::uint32_t pipeline);

    /**
     * @brief Resizes the vertex buffer to 4 points and populates them as a rectangle like a sprite
     *
     * @param texture The texture to use to determine size and texture coords
     */
    void makeSprite(const rc::res::TextureRef& texture);

    /**
     * @brief Returns the default pipeline for regular scenes
     */
    virtual std::uint32_t getDefaultScenePipelineId() const override {
        return getCurrentPipeline();
    }

    /**
     * @brief Returns the default pipeline for overlays
     */
    virtual std::uint32_t getDefaultOverlayPipelineId() const override {
        return getCurrentPipeline();
    }
};

} // namespace pcl
} // namespace bl

#endif
