#include <BLIB/Graphics/VertexBuffer3D.hpp>

namespace bl
{
namespace gfx
{
void VertexBuffer3D::create(engine::World& world, unsigned int vc) {
    Drawable::create(world);
    Transform3D::create(world.engine().ecs(), entity());
    component().create(world.engine(), vc);
}

void VertexBuffer3D::commit() {
    rc::prim::Vertex3D::computeTBN(&component()[0], component().getSize());
    component().commit();
}

void VertexBuffer3D::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace gfx
} // namespace bl
