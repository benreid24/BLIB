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
template<typename TMesh = com::BasicMesh>
class Terrain
: public Drawable<TMesh>
, public bcom::Transform3D {
public:
    /**
     * @brief Creates the uninitialized terrain drawable
     */
    Terrain() = default;

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

template<typename TMesh>
void Terrain<TMesh>::createFromHeightmap(engine::World& world,
                                         const ctr::Vector2D<float>& heightmap,
                                         const glm::vec2& offset, const glm::vec2& size,
                                         const bl::rc::res::MaterialRef& material,
                                         std::uint32_t materialPipelineId) {
    const unsigned int vertexCount = heightmap.getWidth() * heightmap.getHeight();
    const unsigned int indexCount  = (heightmap.getWidth() - 1) * (heightmap.getHeight() - 1) * 6;

    this->createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), this->entity());
    this->component().create(world.engine().renderer(), vertexCount, indexCount);
    updateFromHeightmap(heightmap, offset, size);
}

template<typename TMesh>
void Terrain<TMesh>::updateFromHeightmap(const ctr::Vector2D<float>& heightmap,
                                         const glm::vec2& offset, const glm::vec2& size) {
    const unsigned int xCount      = heightmap.getWidth();
    const unsigned int yCount      = heightmap.getHeight();
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6;

    this->component().gpuBuffer.ensureSize(vertexCount, indexCount);
    auto& indexBuffer = this->component().gpuBuffer;

    for (unsigned int x = 0; x < xCount; ++x) {
        for (unsigned int y = 0; y < yCount; ++y) {
            const float xf =
                static_cast<float>(x) / static_cast<float>(xCount - 1) * size.x + offset.x;
            const float zf =
                static_cast<float>(y) / static_cast<float>(yCount - 1) * size.y + offset.y;
            indexBuffer.vertices()[x + y * xCount].pos = glm::vec3(xf, heightmap(x, y), zf);
        }
    }
    // gen indices
    setIndices(xCount, yCount);

    // finalize and commit
    commitUpdate();
}

template<typename TMesh>
void Terrain<TMesh>::createFromNoise2d(engine::World& world, util::Perlin<float>& perlin,
                                       float width, float height, float altitude, float step,
                                       unsigned int octaves, float persistence, float frequency,
                                       const bl::rc::res::MaterialRef& material,
                                       std::uint32_t materialPipelineId) {
    this->createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), this->entity());
    this->component().create(world.engine().renderer(), 1, 1);
    regenerateFromNoise2d(perlin, width, height, altitude, step, octaves, persistence, frequency);
}

template<typename TMesh>
void Terrain<TMesh>::regenerateFromNoise2d(util::Perlin<float>& perlin, float width, float height,
                                           float altitude, float step, unsigned int octaves,
                                           float persistence, float frequency) {
    const unsigned int xCount      = std::ceil(width / step) + 0.1f;
    const unsigned int yCount      = std::ceil(height / step) + 0.1f;
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6;

    this->component().gpuBuffer.ensureSize(vertexCount, indexCount);
    auto& indexBuffer = this->component().gpuBuffer;

    // gen heightmap
    for (unsigned int x = 0; x < xCount; ++x) {
        for (unsigned int y = 0; y < yCount; ++y) {
            const float xf = static_cast<float>(x) * step - width * 0.5f;
            const float zf = static_cast<float>(y) * step - height * 0.5f;
            float normal =
                perlin.octave2DNormalized(xf * frequency, zf * frequency, octaves, persistence);
            normal                                     = (normal + 1.f) * 0.5f; // map to [0,1]
            indexBuffer.vertices()[x + y * xCount].pos = glm::vec3(xf, normal * altitude, zf);
        }
    }

    // gen indices
    setIndices(xCount, yCount);

    // finalize and commit
    commitUpdate();
}

template<typename TMesh>
template<typename TFunc>
void Terrain<TMesh>::createFromHeightFunction(engine::World& world, TFunc&& heightFunction,
                                              const glm::vec2& offset, const glm::vec2& size,
                                              float step, const bl::rc::res::MaterialRef& material,
                                              std::uint32_t materialPipelineId) {
    const unsigned int xCount      = static_cast<unsigned int>(std::ceil(size.x / step) + 0.1f);
    const unsigned int yCount      = static_cast<unsigned int>(std::ceil(size.y / step) + 0.1f);
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6;

    this->createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), this->entity());
    this->component().create(world.engine().renderer(), vertexCount, indexCount);
    updateFromHeightFunction(std::forward<TFunc>(heightFunction), offset, size, step);
}

template<typename TMesh>
template<typename TFunc>
void Terrain<TMesh>::updateFromHeightFunction(TFunc&& heightFunction, const glm::vec2& offset,
                                              const glm::vec2& size, float step) {
    const unsigned int xCount      = static_cast<unsigned int>(std::ceil(size.x / step) + 0.1f);
    const unsigned int yCount      = static_cast<unsigned int>(std::ceil(size.y / step) + 0.1f);
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6;

    this->component().gpuBuffer.ensureSize(vertexCount, indexCount);
    auto& indexBuffer = this->component().gpuBuffer;

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

template<typename TMesh>
void Terrain<TMesh>::commitUpdate() {
    rc::prim::Vertex3D::computeTBN(this->component().gpuBuffer.vertices().data(),
                                   this->component().gpuBuffer.indices().data(),
                                   this->component().gpuBuffer.indices().size());

    this->component().gpuBuffer.queueTransfer();
    this->component().getDrawParametersForEdit() = this->component().gpuBuffer.getDrawParameters();
}

template<typename TMesh>
void Terrain<TMesh>::setIndices(unsigned int xCount, unsigned int yCount) {
    auto& indexBuffer = this->component().gpuBuffer;
    for (unsigned int x = 0; x < xCount - 1; ++x) {
        for (unsigned int y = 0; y < yCount - 1; ++y) {
            const unsigned int i         = (x + y * (xCount - 1)) * 6;
            indexBuffer.indices()[i + 0] = x + y * xCount;
            indexBuffer.indices()[i + 1] = x + (y + 1) * xCount;
            indexBuffer.indices()[i + 2] = (x + 1) + y * xCount;
            indexBuffer.indices()[i + 3] = (x + 1) + y * xCount;
            indexBuffer.indices()[i + 4] = x + (y + 1) * xCount;
            indexBuffer.indices()[i + 5] = (x + 1) + (y + 1) * xCount;
        }
    }
}

template<typename TMesh>
void Terrain<TMesh>::scaleToSize(const glm::vec2&) {
    BL_LOG_WARN << "Cannot scale terrain to size";
}

} // namespace gfx
} // namespace bl

#endif
