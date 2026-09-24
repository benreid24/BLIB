#include <BLIB/Graphics/Cylinder.hpp>

#include <BLIB/Math.hpp>

namespace bl
{
namespace gfx
{
Cylinder::Cylinder() {}

Cylinder::Cylinder(engine::World& world, float height, float radius, unsigned int pointCount,
                   const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    create(world, height, radius, pointCount, material, materialPipelineId);
}

void Cylinder::create(engine::World& world, float height, float radius, unsigned int pointCount,
                      const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    Drawable::createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), entity());
    Outline3D::init(world.engine().ecs(), entity(), &component());

    const unsigned int circleVertexCount = pointCount + 1;
    const unsigned int circleIndexCount  = pointCount * 3;
    const unsigned int sideVertexCount   = pointCount * 2;
    const unsigned int sideIndexCount    = pointCount * 6;
    component().create(world.engine().renderer(),
                       sideVertexCount + circleVertexCount * 2,
                       circleIndexCount * 2 + sideIndexCount);

    const float h        = height * 0.5f;
    const float pc       = static_cast<float>(pointCount);
    unsigned int vbIndex = 0;
    auto& vb             = component().gpuBuffer.vertices();

    unsigned int ibIndex = 0;
    auto& ib             = component().gpuBuffer.indices();

    const auto makePoint = [pc, radius](unsigned int i, float y) -> glm::vec3 {
        const float r = static_cast<float>(i) / pc * 2.f * math::Pi;
        return glm::vec3(std::cos(r) * radius, y, std::sin(r) * radius);
    };

    // side faces
    for (unsigned int i = 0; i < pointCount; i += 2) {
        const unsigned int i2 = (i + 1) % pointCount;
        vb[vbIndex + 0].pos   = makePoint(i, -h);
        vb[vbIndex + 1].pos   = makePoint(i, h);
        vb[vbIndex + 2].pos   = makePoint(i2, h);
        vb[vbIndex + 3].pos   = makePoint(i2, -h);

        ib[ibIndex + 0] = vbIndex;
        ib[ibIndex + 1] = vbIndex + 1;
        ib[ibIndex + 2] = vbIndex + 2;
        ib[ibIndex + 3] = vbIndex;
        ib[ibIndex + 4] = vbIndex + 2;
        ib[ibIndex + 5] = vbIndex + 3;

        vbIndex += 4;
        ibIndex += 6;
    }

    // top and bottom circle faces
    const auto makeCircle = [&vb, &ib, &vbIndex, &ibIndex, &makePoint, pointCount](float h) {
        const unsigned int centerIndex = vbIndex;
        vb[centerIndex].pos            = {0.f, h, 0.f};
        ++vbIndex;

        for (unsigned int i = 0; i < pointCount; i += 2) {
            vb[vbIndex + 0].pos = makePoint(i, h);
            vb[vbIndex + 1].pos = makePoint((i + 1) % pointCount, h);

            ib[ibIndex + 0] = centerIndex;
            ib[ibIndex + 1] = vbIndex + 0;
            ib[ibIndex + 2] = vbIndex + 1;

            vbIndex += 2;
            ibIndex += 3;
        }
    };

    makeCircle(h);
    makeCircle(-h);

    // preprocessing
    rc::prim::Vertex3D::computeTBN(component().gpuBuffer.vertices().data(),
                                   component().gpuBuffer.indices().data(),
                                   component().gpuBuffer.indices().size());

    component().gpuBuffer.queueTransfer();
}

void Cylinder::setColor(const rc::Color& c) {
    for (auto& v : component().gpuBuffer.vertices()) { v.color = c.toVec4(); }
    component().gpuBuffer.queueTransfer();
}

void Cylinder::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace gfx
} // namespace bl
