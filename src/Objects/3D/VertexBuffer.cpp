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

} // namespace o3d
} // namespace obj
} // namespace bl
