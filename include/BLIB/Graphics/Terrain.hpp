#ifndef BLIB_GRAPHICS_TERRAIN_HPP
#define BLIB_GRAPHICS_TERRAIN_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Containers/Vector2d.hpp>
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
     * @brief Creates the terrain from a heightmap
     *
     * @param world The world to create the terrain in
     * @param heightmap The heightmap to use for the terrain
     * @param offset The offset of the upper left corner in world coordinates
     * @param size The size of the terrain in world coordinates
     * @param material The material to assign to the terrain
     * @param materialPipelineId The id of the material pipeline to render with
     */
    void createFromHeightmap(
        engine::World& world, const ctr::Vector2D<float>& heightmap, const glm::vec2& offset,
        const glm::vec2& size, const bl::rc::res::MaterialRef& material = {},
        std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Creates the terrain from a height function
     *
     * @tparam TFunc The type of the height function. Should be float(glm::vec2)
     * @param world The world to create the terrain in
     * @param heightFunction The height function to use
     * @param offset The offset of the upper left corner in world coordinates
     * @param size The size of the terrain in world coordinates
     * @param step The distance between each vertex in world coordinates
     * @param material The material to assign to the terrain
     * @param materialPipelineId The id of the material pipeline to render with
     */
    template<typename TFunc>
    void createFromHeightFunction(
        engine::World& world, TFunc&& heightFunction, const glm::vec2& offset,
        const glm::vec2& size, float step, const bl::rc::res::MaterialRef& material = {},
        std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3D);

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
     * @brief Updates the terrain mesh from a heightmap
     *
     * @param heightmap The heightmap to use
     * @param offset The offset of the upper left corner in world coordinates
     * @param size The size of the terrain in world coordinates
     */
    void updateFromHeightmap(const ctr::Vector2D<float>& heightmap, const glm::vec2& offset,
                             const glm::vec2& size);

    /**
     * @brief Updates the terrain mesh from a height function
     *
     * @tparam TFunc The type of the height function. Should be float(glm::vec2)
     * @param heightFunction The height function to use
     * @param offset The offset of the upper left corner in world coordinates
     * @param size The size of the terrain in world coordinates
     * @param step The distance between each vertex in world coordinates
     */
    template<typename TFunc>
    void updateFromHeightFunction(TFunc&& heightFunction, const glm::vec2& offset,
                                  const glm::vec2& size, float step);

    /**
     * @brief Recomputes TBN, queues buffers for transfer, and updates draw parameters
     */
    void commitUpdate();

private:
    virtual void scaleToSize(const glm::vec2& size) override;
    void setIndices(unsigned int xCount, unsigned int yCount);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TFunc>
void Terrain::createFromHeightFunction(engine::World& world, TFunc&& heightFunction,
                                       const glm::vec2& offset, const glm::vec2& size, float step,
                                       const bl::rc::res::MaterialRef& material,
                                       std::uint32_t materialPipelineId) {
    const unsigned int xCount      = static_cast<unsigned int>(std::ceil(size.x / step) + 0.1f);
    const unsigned int yCount      = static_cast<unsigned int>(std::ceil(size.y / step) + 0.1f);
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6;

    Drawable::createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), entity());
    component().create(world.engine().renderer(), vertexCount, indexCount);
    updateFromHeightFunction(std::forward<TFunc>(heightFunction), offset, size, step);
}

template<typename TFunc>
void Terrain::updateFromHeightFunction(TFunc&& heightFunction, const glm::vec2& offset,
                                       const glm::vec2& size, float step) {
    const unsigned int xCount      = static_cast<unsigned int>(std::ceil(size.x / step) + 0.1f);
    const unsigned int yCount      = static_cast<unsigned int>(std::ceil(size.y / step) + 0.1f);
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6;

    component().gpuBuffer.ensureSize(vertexCount, indexCount);
    auto& indexBuffer = component().gpuBuffer;

    for (unsigned int x = 0; x < xCount; ++x) {
        for (unsigned int y = 0; y < yCount; ++y) {
            const float xf =
                static_cast<float>(x) / static_cast<float>(xCount - 1) * size.x + offset.x;
            const float zf =
                static_cast<float>(y) / static_cast<float>(yCount - 1) * size.y + offset.y;
            indexBuffer.vertices()[x + y * xCount].pos =
                glm::vec3(xf, heightFunction(glm::vec2(xf, zf)), zf);
        }
    }
    // gen indices
    setIndices(xCount, yCount);

    // finalize and commit
    commitUpdate();
}

} // namespace gfx
} // namespace bl

#endif
