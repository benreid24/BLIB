#ifndef BLIB_GRAPHICS_TERRAIN_HPP
#define BLIB_GRAPHICS_TERRAIN_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Graphics/Components/Transform3D.hpp>
#include <BLIB/Graphics/Drawable.hpp>
#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>
#include <BLIB/Util/Perlin.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Basic single mesh for terrain with perlin noise generation
 *
 * @ingroup Graphics
 */
class Terrain
: public Drawable<com::BasicMesh>
, public bcom::Transform3D {
public:
    /**
     * @brief Creates the uninitialized terrain drawable
     */
    Terrain();

    /**
     * @brief Creates the terrain using Perlin noise centered around the origin (0, 0, 0)
     *
     * @param world The world to create the terrain in
     * @param perlin The seeded Perlin noise generator
     * @param width The width of the terrain in world coordinates
     * @param height The height of the terrain in world coordinates
     * @param altitude The max height of the terrain to generate
     * @param step The distance between each vertex in world coordinates
     * @param octaves The number of Perlin octaves to use during generation
     * @param persistence The persistence value to use for amplitude scaling during generation
     * @param frequency The frequency value to use for scaling during generation
     * @param material The material to assign to the terrain
     * @param materialPipelineId The id of the material pipeline to render with
     */
    void createFromNoise2d(engine::World& world, util::Perlin<float>& perlin, float width,
                           float height, float altitude, float step, unsigned int octaves,
                           float persistence, float frequency,
                           const bl::rc::res::MaterialRef& material = {},
                           std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Regenerates the terrain using Perlin noise centered around the origin (0, 0, 0)
     *
     * @param perlin The seeded Perlin noise generator
     * @param width The width of the terrain in world coordinates
     * @param height The height of the terrain in world coordinates
     * @param altitude The max height of the terrain to generate
     * @param step The distance between each vertex in world coordinates
     * @param octaves The number of Perlin octaves to use during generation
     * @param persistence The persistence value to use for amplitude scaling during generation
     * @param frequency The frequency value to use for scaling during generation
     */
    void regenerateFromNoise2d(util::Perlin<float>& perlin, float width, float height,
                               float altitude, float step, unsigned int octaves, float persistence,
                               float frequency);

    /**
     * @brief Recomputes TBN, queues buffers for transfer, and updates draw parameters
     */
    void commitUpdate();

private:
    virtual void scaleToSize(const glm::vec2& size) override;
};

} // namespace gfx
} // namespace bl

#endif
