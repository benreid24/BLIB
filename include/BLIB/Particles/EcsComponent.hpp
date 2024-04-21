#ifndef BLIB_PARTICLES_ECSCOMPONENT_HPP
#define BLIB_PARTICLES_ECSCOMPONENT_HPP

#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

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
};

} // namespace pcl
} // namespace bl

#endif
