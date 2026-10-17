#include <BLIB/Objects/3D/VertexBuffer.hpp>

namespace bl
{
namespace obj
{
namespace o3d
{
void VertexBuffer::create(engine::World& world, unsigned int vc) {
    Drawable::create(world);
    Transform3D::create(world.engine().ecs(), entity());
    Outline3D::init(world.engine().ecs(), entity(), &component());
    component().create(world.engine(), vc);
}

void VertexBuffer::commit() {
    rc::prim::Vertex3D::computeTBN(&component()[0], component().getSize());
    component().commit();
}

void VertexBuffer::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace o3d
} // namespace obj
} // namespace bl
