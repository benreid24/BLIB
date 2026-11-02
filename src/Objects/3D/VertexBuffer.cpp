#include <BLIB/Objects/3D/VertexBuffer.hpp>

namespace bl
{
namespace obj
{
namespace o3d
{
void VertexBuffer::create(engine::World& world, unsigned int vc) {
    DrawableObject3D::create(world);
    component().create(world.engine(), vc);
}

void VertexBuffer::commit() {
    rc::prim::Vertex3D::computeTBN(&component()[0], component().getSize());
    component().commit();
}

std::unique_ptr<JPH::Shape> VertexBuffer::createPhysicsShape() {
    std::vector<JPH::Vec3> points;
    points.reserve(component().getSize());

    for (unsigned int i = 0; i < component().getSize(); ++i) {
        points.emplace_back(component()[i].pos.x, component()[i].pos.y, component()[i].pos.z);
    }

    JPH::ConvexHullShapeSettings settings(points.data(), points.size());
    JPH::Shape::ShapeResult result;
    return std::make_unique<JPH::ConvexHullShape>(settings, result);
}

} // namespace o3d
} // namespace obj
} // namespace bl
