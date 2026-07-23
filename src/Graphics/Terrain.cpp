#include <BLIB/Graphics/Terrain.hpp>

namespace bl
{
namespace gfx
{
Terrain::Terrain() {}

void Terrain::createFromNoise2d(engine::World& world, util::Perlin<float>& perlin, float width,
                                float height, float altitude, float step, unsigned int octaves,
                                float persistence, float frequency,
                                const bl::rc::res::MaterialRef& material,
                                std::uint32_t materialPipelineId) {
    Drawable::createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), entity());
    component().create(world.engine().renderer(), 1, 1);
    regenerateFromNoise2d(perlin, width, height, altitude, step, octaves, persistence, frequency);
}

void Terrain::regenerateFromNoise2d(util::Perlin<float>& perlin, float width, float height,
                                    float altitude, float step, unsigned int octaves,
                                    float persistence, float frequency) {
    const unsigned int xCount      = std::ceil(width / step) + 0.1f;
    const unsigned int yCount      = std::ceil(height / step) + 0.1f;
    const unsigned int vertexCount = xCount * yCount;
    const unsigned int indexCount  = (xCount - 1) * (yCount - 1) * 6; // TODO - correct?

    component().gpuBuffer.ensureSize(vertexCount, indexCount);
    auto& indexBuffer = component().gpuBuffer;

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

    // finalize and commit
    commitUpdate();
}

void Terrain::commitUpdate() {
    rc::prim::Vertex3D::computeTBN(component().gpuBuffer.vertices().data(),
                                   component().gpuBuffer.indices().data(),
                                   component().gpuBuffer.indices().size());

    component().gpuBuffer.queueTransfer();
    component().getDrawParametersForEdit() = component().gpuBuffer.getDrawParameters();
}

void Terrain::scaleToSize(const glm::vec2&) { BL_LOG_WARN << "Cannot scale terrain to size"; }

} // namespace gfx
} // namespace bl
