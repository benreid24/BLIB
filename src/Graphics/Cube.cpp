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
    component().create(world.engine().renderer().vulkanState(), 24, 36);

    const float s = size * 0.5f;

    // top vertices
    component().gpuBuffer.vertices()[0].pos = {-s, s, -s}; // back left
    component().gpuBuffer.vertices()[1].pos = {s, s, -s};  // back right
    component().gpuBuffer.vertices()[2].pos = {s, s, s};   // front right
    component().gpuBuffer.vertices()[3].pos = {-s, s, s};  // front left

    // right vertices
    component().gpuBuffer.vertices()[4].pos = {s, s, s};   // front top
    component().gpuBuffer.vertices()[5].pos = {s, s, -s};  // back top
    component().gpuBuffer.vertices()[6].pos = {s, -s, -s}; // back bottom
    component().gpuBuffer.vertices()[7].pos = {s, -s, s};  // front bottom

    // bottom vertices
    component().gpuBuffer.vertices()[8].pos  = {-s, -s, s};  // front left
    component().gpuBuffer.vertices()[9].pos  = {s, -s, s};   // front right
    component().gpuBuffer.vertices()[10].pos = {s, -s, -s};  // back right
    component().gpuBuffer.vertices()[11].pos = {-s, -s, -s}; // back left

    // left vertices
    component().gpuBuffer.vertices()[12].pos = {-s, s, -s};  // back top
    component().gpuBuffer.vertices()[13].pos = {-s, s, s};   // front top
    component().gpuBuffer.vertices()[14].pos = {-s, -s, s};  // front bottom
    component().gpuBuffer.vertices()[15].pos = {-s, -s, -s}; // back bottom

    // front vertices
    component().gpuBuffer.vertices()[16].pos = {-s, s, s};  // top left
    component().gpuBuffer.vertices()[17].pos = {s, s, s};   // top right
    component().gpuBuffer.vertices()[18].pos = {s, -s, s};  // bottom right
    component().gpuBuffer.vertices()[19].pos = {-s, -s, s}; // bottom left

    // back vertices
    component().gpuBuffer.vertices()[20].pos = {s, s, -s};   // top right
    component().gpuBuffer.vertices()[21].pos = {-s, s, -s};  // top left
    component().gpuBuffer.vertices()[22].pos = {-s, -s, -s}; // bottom left
    component().gpuBuffer.vertices()[23].pos = {s, -s, -s};  // bottom right

    // faces
    for (unsigned int face = 0; face < 6; ++face) {
        const unsigned int vb                             = face * 4;
        component().gpuBuffer.vertices()[vb + 0].texCoord = glm::vec2(0.f, 0.f);
        component().gpuBuffer.vertices()[vb + 1].texCoord = glm::vec2(1.f, 0.f);
        component().gpuBuffer.vertices()[vb + 2].texCoord = glm::vec2(1.f, 1.f);
        component().gpuBuffer.vertices()[vb + 3].texCoord = glm::vec2(0.f, 1.f);

        const unsigned int b                   = face * 6;
        component().gpuBuffer.indices()[b + 0] = vb + 0;
        component().gpuBuffer.indices()[b + 1] = vb + 2;
        component().gpuBuffer.indices()[b + 2] = vb + 1;
        component().gpuBuffer.indices()[b + 3] = vb + 0;
        component().gpuBuffer.indices()[b + 4] = vb + 3;
        component().gpuBuffer.indices()[b + 5] = vb + 2;
    }

    // Right (+X)
    component().gpuBuffer.vertices()[4].texCoord = glm::vec2(0.f, 1.f);
    component().gpuBuffer.vertices()[5].texCoord = glm::vec2(1.f, 1.f);
    component().gpuBuffer.vertices()[6].texCoord = glm::vec2(1.f, 0.f);
    component().gpuBuffer.vertices()[7].texCoord = glm::vec2(0.f, 0.f);

    // Left (-X)
    component().gpuBuffer.vertices()[12].texCoord = glm::vec2(0.f, 1.f);
    component().gpuBuffer.vertices()[13].texCoord = glm::vec2(1.f, 1.f);
    component().gpuBuffer.vertices()[14].texCoord = glm::vec2(1.f, 0.f);
    component().gpuBuffer.vertices()[15].texCoord = glm::vec2(0.f, 0.f);

    // preprocessing
    rc::prim::Vertex3D::computeTBN(component().gpuBuffer.vertices().data(),
                                   component().gpuBuffer.indices().data(),
                                   component().gpuBuffer.indices().size());

    component().gpuBuffer.queueTransfer();
}

void Cube::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace gfx
} // namespace bl
