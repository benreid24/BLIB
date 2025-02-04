#include <BLIB/Graphics/Cube.hpp>

namespace bl
{
namespace gfx
{
Cube::Cube() {}

Cube::Cube(engine::World& world, float size, const bl::rc::res::MaterialRef& material,
           std::uint32_t materialPipelineId) {
    Cube::create(world, size, material, materialPipelineId);
}

void Cube::create(engine::World& world, float size, const bl::rc::res::MaterialRef& material,
                  std::uint32_t materialPipelineId) {
    Drawable::createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), entity());
    component().create(world.engine().renderer().vulkanState(), 8, 36);

    const float s = size * 0.5f;
    const auto face =
        [this](unsigned int i, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) {
            const unsigned int b                   = i * 6;
            component().gpuBuffer.indices()[b + 0] = v0;
            component().gpuBuffer.indices()[b + 1] = v1;
            component().gpuBuffer.indices()[b + 2] = v2;
            component().gpuBuffer.indices()[b + 3] = v0;
            component().gpuBuffer.indices()[b + 4] = v2;
            component().gpuBuffer.indices()[b + 5] = v3;
        };

    // top vertices
    component().gpuBuffer.vertices()[0].pos      = {-s, -s, s}; // back left
    component().gpuBuffer.vertices()[0].texCoord = {0.f, 0.f};
    component().gpuBuffer.vertices()[1].pos      = {s, -s, s}; // back right
    component().gpuBuffer.vertices()[1].texCoord = {1.f, 0.f};
    component().gpuBuffer.vertices()[2].pos      = {s, -s, -s}; // front right
    component().gpuBuffer.vertices()[2].texCoord = {1.f, 1.f};
    component().gpuBuffer.vertices()[3].pos      = {-s, -s, -s}; // front left
    component().gpuBuffer.vertices()[3].texCoord = {0.f, 1.f};

    // bottom vertices
    component().gpuBuffer.vertices()[4].pos      = {-s, s, s}; // back left
    component().gpuBuffer.vertices()[4].texCoord = {0.f, 0.f};
    component().gpuBuffer.vertices()[5].pos      = {s, s, s}; // back right
    component().gpuBuffer.vertices()[5].texCoord = {1.f, 0.f};
    component().gpuBuffer.vertices()[6].pos      = {s, s, -s}; // front right
    component().gpuBuffer.vertices()[6].texCoord = {1.f, 1.f};
    component().gpuBuffer.vertices()[7].pos      = {-s, s, -s}; // front left
    component().gpuBuffer.vertices()[7].texCoord = {0.f, 1.f};

    // faces
    face(0, 0, 1, 2, 3); // top
    face(1, 1, 5, 6, 2); // right
    face(2, 7, 6, 5, 4); // bottom
    face(3, 0, 4, 7, 3); // left
    face(4, 3, 2, 6, 7); // front
    face(5, 1, 0, 4, 5); // back

    component().gpuBuffer.queueTransfer();
}

void Cube::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace gfx
} // namespace bl
