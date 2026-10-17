#ifndef BLIB_OBJECTS_3D_HEIGHTMAPTERRAIN_HPP
#define BLIB_OBJECTS_3D_HEIGHTMAPTERRAIN_HPP

#include <BLIB/Components/Heightmap.hpp>
#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Objects/3D/DrawableObject3D.hpp>
#include <BLIB/Random/Perlin.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace obj
{
namespace o3d
{
/**
 * @brief Basic single mesh for heightmap based terrain
 *
 * @ingroup Objects
 */
template<typename TVertex, std::uint32_t MaterialPipelineId>
class HeightmapTerrain : public DrawableObject3D<com::Mesh<TVertex, MaterialPipelineId>> {
public:
    /**
     * @brief Default callback for populating a vertex from a position
     *
     * @param pos The position of the vertex
     * @param vertex The vertex to populate
     */
    static void populateVertexDefault(const glm::vec3& pos, TVertex& vertex);

    /**
     * @brief Creates the uninitialized terrain drawable
     */
    HeightmapTerrain();

    /**
     * @brief Creates the terrain vertices and indices but does not populate vertices
     *
     * @param world The world to create the terrain in
     * @param offset The offset of the upper left corner in world coordinates
     * @param size The size of the terrain in world coordinates
     * @param material The material to assign to the terrain
     * @param materialPipelineId The id of the material pipeline to render with
     */
    void createEmptyGrid(engine::World& world, const glm::vec2& offset, const glm::vec2& size,
                         float step, const bl::rc::res::MaterialRef& material = {},
                         std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3D);

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
    void createFromNoise2d(engine::World& world, rand::Perlin<float>& perlin, float width,
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
    void regenerateFromNoise2d(rand::Perlin<float>& perlin, float width, float height,
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
     * @brief Updates the terrain mesh from a heightmap
     *
     * @tparam TFunc The type of the callback function. Should be void(const glm::vec3&, TVertex&)
     * @param heightmap The heightmap to use
     * @param offset The offset of the upper left corner in world coordinates
     * @param size The size of the terrain in world coordinates
     * @param populateVertexCb The callback function to populate the vertex data from the heightmap
     */
    template<typename TFunc>
    void updateFromHeightmap(const ctr::Vector2D<float>& heightmap, const glm::vec2& offset,
                             const glm::vec2& size, TFunc&& populateVertexCb);

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
     * @brief Returns the heightmap component for this terrain
     */
    const com::Heightmap& getHeightmap() const { return *heightmap; }

    /**
     * @brief Synchronizes changes to the heightmap to the mesh
     *
     * @tparam TFunc The type of the callback function. Should be void(const glm::vec3&, TVertex&)
     * @param populateVertexCb The callback function to populate the vertex data from the heightmap
     */
    template<typename TFunc>
    void syncFromHeightmap(TFunc&& populateVertexCb);

    /**
     * @brief Synchronizes changes to the heightmap to the mesh using default vertex population
     */
    void syncFromHeightmap();

    /**
     * @brief Recomputes TBN, queues buffers for transfer, and updates draw parameters
     */
    void commitUpdate();

private:
    com::Heightmap* heightmap;

    void createHeightmapComponent(engine::World& world);
    void setIndices(unsigned int xCount, unsigned int yCount);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TVertex, std::uint32_t MaterialPipelineId>
HeightmapTerrain<TVertex, MaterialPipelineId>::HeightmapTerrain()
: heightmap(nullptr) {}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::createEmptyGrid(
    engine::World& world, const glm::vec2& offset, const glm::vec2& size, float step,
    const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    const unsigned int xCount      = static_cast<unsigned int>(std::ceil(size.x / step) + 0.1f);
    const unsigned int yCount      = static_cast<unsigned int>(std::ceil(size.y / step) + 0.1f);
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6;

    this->createWithMaterial(world, materialPipelineId, material);
    this->component().create(world.engine().renderer(), vertexCount, indexCount);

    createHeightmapComponent(world);
    heightmap->create(offset, size, step);
    heightmap->heightmap.fill(0.f);

    syncFromHeightmap();
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::createFromHeightmap(
    engine::World& world, const ctr::Vector2D<float>& heightmap, const glm::vec2& offset,
    const glm::vec2& size, const bl::rc::res::MaterialRef& material,
    std::uint32_t materialPipelineId) {
    const unsigned int vertexCount = heightmap.getWidth() * heightmap.getHeight();
    const unsigned int indexCount  = (heightmap.getWidth() - 1) * (heightmap.getHeight() - 1) * 6;

    this->createWithMaterial(world, materialPipelineId, material);
    this->component().create(world.engine().renderer(), vertexCount, indexCount);
    createHeightmapComponent(world);
    updateFromHeightmap(heightmap, offset, size);
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::updateFromHeightmap(
    const ctr::Vector2D<float>& heightmap, const glm::vec2& offset, const glm::vec2& size) {
    this->heightmap->create(heightmap, offset, size);
    syncFromHeightmap();
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
template<typename TFunc>
void HeightmapTerrain<TVertex, MaterialPipelineId>::updateFromHeightmap(
    const ctr::Vector2D<float>& heightmap, const glm::vec2& offset, const glm::vec2& size,
    TFunc&& populateVertexCb) {
    this->heightmap->create(heightmap, offset, size);
    syncFromHeightmap(populateVertexCb);
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::createFromNoise2d(
    engine::World& world, rand::Perlin<float>& perlin, float width, float height, float altitude,
    float step, unsigned int octaves, float persistence, float frequency,
    const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    this->createWithMaterial(world, materialPipelineId, material);
    this->component().create(world.engine().renderer(), 1, 1);
    createHeightmapComponent(world);
    regenerateFromNoise2d(perlin, width, height, altitude, step, octaves, persistence, frequency);
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::regenerateFromNoise2d(
    rand::Perlin<float>& perlin, float width, float height, float altitude, float step,
    unsigned int octaves, float persistence, float frequency) {
    const unsigned int xCount = std::ceil(width / step) + 0.1f;
    const unsigned int yCount = std::ceil(height / step) + 0.1f;

    const glm::vec2 size(width, height);
    heightmap->create(-size * 0.5f, size, step);

    // gen heightmap
    for (unsigned int x = 0; x < xCount; ++x) {
        for (unsigned int y = 0; y < yCount; ++y) {
            const float xf = static_cast<float>(x) * step - width * 0.5f;
            const float zf = static_cast<float>(y) * step - height * 0.5f;
            float normal =
                perlin.octave2DNormalized(xf * frequency, zf * frequency, octaves, persistence);
            normal                     = (normal + 1.f) * 0.5f; // map to [0,1]
            heightmap->heightmap(x, y) = normal * altitude;
        }
    }

    syncFromHeightmap();
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
template<typename TFunc>
void HeightmapTerrain<TVertex, MaterialPipelineId>::createFromHeightFunction(
    engine::World& world, TFunc&& heightFunction, const glm::vec2& offset, const glm::vec2& size,
    float step, const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    const unsigned int xCount      = static_cast<unsigned int>(std::ceil(size.x / step) + 0.1f);
    const unsigned int yCount      = static_cast<unsigned int>(std::ceil(size.y / step) + 0.1f);
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6;

    this->createWithMaterial(world, materialPipelineId, material);
    this->component().create(world.engine().renderer(), vertexCount, indexCount);
    createHeightmapComponent(world);
    updateFromHeightFunction(std::forward<TFunc>(heightFunction), offset, size, step);
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
template<typename TFunc>
void HeightmapTerrain<TVertex, MaterialPipelineId>::updateFromHeightFunction(
    TFunc&& heightFunction, const glm::vec2& offset, const glm::vec2& size, float step) {
    const unsigned int xCount = static_cast<unsigned int>(std::ceil(size.x / step) + 0.1f);
    const unsigned int yCount = static_cast<unsigned int>(std::ceil(size.y / step) + 0.1f);

    heightmap->create(offset, size, step);
    this->size   = size;
    this->offset = offset;

    for (unsigned int x = 0; x < xCount; ++x) {
        for (unsigned int y = 0; y < yCount; ++y) {
            const float xf =
                static_cast<float>(x) / static_cast<float>(xCount - 1) * size.x + offset.x;
            const float zf =
                static_cast<float>(y) / static_cast<float>(yCount - 1) * size.y + offset.y;
            heightmap->heightmap(x, y) = heightFunction(glm::vec2(xf, zf));
        }
    }

    syncFromHeightmap();
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::syncFromHeightmap() {
    syncFromHeightmap(&populateVertexDefault);
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
template<typename TFunc>
void HeightmapTerrain<TVertex, MaterialPipelineId>::syncFromHeightmap(TFunc&& populateVertex) {
    const glm::vec2& size          = heightmap->size;
    const glm::vec2& offset        = heightmap->offset;
    const unsigned int xCount      = heightmap->heightmap.getWidth();
    const unsigned int yCount      = heightmap->heightmap.getHeight();
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
            const glm::vec3 pos(xf, heightmap->heightmap(x, y), zf);
            populateVertex(pos, indexBuffer.vertices()[x + y * xCount]);
        }
    }

    // gen indices
    setIndices(xCount, yCount);

    // finalize and commit
    commitUpdate();
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::populateVertexDefault(const glm::vec3& pos,
                                                                          TVertex& v) {
    v.pos = pos;
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::commitUpdate() {
    TVertex::computeTBN(this->component().gpuBuffer.vertices().data(),
                        this->component().gpuBuffer.indices().data(),
                        this->component().gpuBuffer.indices().size());

    this->component().gpuBuffer.queueTransfer();
    this->component().getDrawParametersForEdit() = this->component().gpuBuffer.getDrawParameters();
}

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::setIndices(unsigned int xCount,
                                                               unsigned int yCount) {
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

template<typename TVertex, std::uint32_t MaterialPipelineId>
void HeightmapTerrain<TVertex, MaterialPipelineId>::createHeightmapComponent(engine::World& world) {
    heightmap = world.engine().ecs().emplaceComponent<com::Heightmap>(this->entity());
}

} // namespace o3d
} // namespace obj
} // namespace bl

#endif
