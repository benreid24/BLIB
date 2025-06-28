#include <BLIB/Graphics/Cone.hpp>

#include <BLIB/Math.hpp>

namespace bl
{
namespace gfx
{
Cone::Cone() {}

Cone::Cone(engine::World& world, float height, float radius, unsigned int pointCount,
           const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    create(world, height, radius, pointCount, material, materialPipelineId);
}

void Cone::create(engine::World& world, float height, float radius, unsigned int pointCount,
                  const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    Drawable::createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), entity());
    Outline3D::init(world.engine().ecs(), entity(), &component());
    component().create(world.engine().renderer().vulkanState(), pointCount * 4 + 1, pointCount * 6);

    const float h  = height * 0.5f;
    const float pc = static_cast<float>(pointCount);
    auto& vb       = component().gpuBuffer.vertices();

    unsigned int ii = 0;
    auto& ib        = component().gpuBuffer.indices();

    const auto makePoint = [pc, radius, h](unsigned int i) -> glm::vec3 {
        const float r = static_cast<float>(i) / pc * 2.f * math::Pi;
        return glm::vec3(std::cos(r) * radius, std::sin(r) * radius, h);
    };

    // cone faces
    for (unsigned int i = 0; i < pointCount; ++i) {
        const unsigned int b = i * 3;

        const glm::vec4 color =
            i % 2 == 0 ? glm::vec4{0.f, 1.f, 0.f, 1.f} : glm::vec4{0.f, 0.f, 1.f, 1.f};
        vb[b].color     = color;
        vb[b + 1].color = color;
        vb[b + 2].color = color;

        vb[b + 0].pos = {0.f, 0.f, -h};
        vb[b + 1].pos = makePoint(i);
        vb[b + 2].pos = makePoint(i + 1);

        ib[ii + 0] = b;
        ib[ii + 1] = b + 2;
        ib[ii + 2] = b + 1;
        ii += 3;
    }

    // bottom face
    const unsigned int cb = pointCount * 3;
    vb[cb].pos            = {0.f, 0.f, h};
    for (unsigned int i = 0; i < pointCount; ++i) {
        const unsigned int b = cb + i + 1;
        vb[b].pos            = makePoint(i);
        vb[b].color          = {1.f, 0.f, 0.f, 1.f};

        ib[ii + 0] = cb;
        ib[ii + 1] = b + 0;
        ib[ii + 2] = (i != pointCount - 1) ? b + 1 : cb + 1;
        ii += 3;
    }

    // preprocessing
    rc::prim::Vertex3D::computeTBN(component().gpuBuffer.vertices().data(),
                                   component().gpuBuffer.indices().data(),
                                   component().gpuBuffer.indices().size());

    component().gpuBuffer.queueTransfer();
}

void Cone::setColor(const rc::Color& c) {
    for (auto& v : component().gpuBuffer.vertices()) { v.color = c.toVec4(); }
    component().gpuBuffer.queueTransfer();
}

void Cone::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace gfx
} // namespace bl
