#include <BLIB/Graphics/RectangularPrism.hpp>

namespace bl
{
namespace gfx
{
RectangularPrism::RectangularPrism() {}

RectangularPrism::RectangularPrism(engine::World& world, float length,
                                   const glm::vec2& crossSection,
                                   const bl::rc::res::MaterialRef& material,
                                   std::uint32_t materialPipelineId) {
    RectangularPrism::create(world, length, crossSection, material, materialPipelineId);
}

void RectangularPrism::create(engine::World& world, float length, const glm::vec2& crossSection,
                              const bl::rc::res::MaterialRef& material,
                              std::uint32_t materialPipelineId) {
    Drawable::createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), entity());
    Outline3D::init(world.engine().ecs(), entity(), &component());
    component().create(world.engine().renderer(), 24, 36);

    const float hx = crossSection.x * 0.5f;
    const float hy = crossSection.y * 0.5f;
    const float hz = length * 0.5f;

    // top vertices
    component().gpuBuffer.vertices()[0].pos = {-hx, hy, -hz}; // back left
    component().gpuBuffer.vertices()[1].pos = {hx, hy, -hz};  // back right
    component().gpuBuffer.vertices()[2].pos = {hx, hy, hz};   // front right
    component().gpuBuffer.vertices()[3].pos = {-hx, hy, hz};  // front left

    // right vertices
    component().gpuBuffer.vertices()[4].pos = {hx, hy, hz};   // front top
    component().gpuBuffer.vertices()[5].pos = {hx, hy, -hz};  // back top
    component().gpuBuffer.vertices()[6].pos = {hx, -hy, -hz}; // back bottom
    component().gpuBuffer.vertices()[7].pos = {hx, -hy, hz};  // front bottom

    // bottom vertices
    component().gpuBuffer.vertices()[8].pos  = {-hx, -hy, hz};  // front left
    component().gpuBuffer.vertices()[9].pos  = {hx, -hy, hz};   // front right
    component().gpuBuffer.vertices()[10].pos = {hx, -hy, -hz};  // back right
    component().gpuBuffer.vertices()[11].pos = {-hx, -hy, -hz}; // back left

    // left vertices
    component().gpuBuffer.vertices()[12].pos = {-hx, hy, -hz};  // back top
    component().gpuBuffer.vertices()[13].pos = {-hx, hy, hz};   // front top
    component().gpuBuffer.vertices()[14].pos = {-hx, -hy, hz};  // front bottom
    component().gpuBuffer.vertices()[15].pos = {-hx, -hy, -hz}; // back bottom

    // front vertices
    component().gpuBuffer.vertices()[16].pos = {-hx, hy, hz};  // top left
    component().gpuBuffer.vertices()[17].pos = {hx, hy, hz};   // top right
    component().gpuBuffer.vertices()[18].pos = {hx, -hy, hz};  // bottom right
    component().gpuBuffer.vertices()[19].pos = {-hx, -hy, hz}; // bottom left

    // back vertices
    component().gpuBuffer.vertices()[20].pos = {hx, hy, -hz};   // top right
    component().gpuBuffer.vertices()[21].pos = {-hx, hy, -hz};  // top left
    component().gpuBuffer.vertices()[22].pos = {-hx, -hy, -hz}; // bottom left
    component().gpuBuffer.vertices()[23].pos = {hx, -hy, -hz};  // bottom right

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

void RectangularPrism::scaleToSize(const glm::vec2&) {
    // noop
}

void RectangularPrism::setColor(const rc::Color& color) {
    for (std::size_t i = 0; i < component().gpuBuffer.vertices().size(); ++i) {
        component().gpuBuffer.vertices()[i].color = color;
    }
    component().gpuBuffer.queueTransfer();
}

} // namespace gfx
} // namespace bl
