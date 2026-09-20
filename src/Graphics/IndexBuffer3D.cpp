#include <BLIB/Graphics/IndexBuffer3D.hpp>

namespace bl
{
namespace gfx
{
void IndexBuffer3D::create(engine::World& world, unsigned int vc, unsigned int ic) {
    Drawable::create(world);
    Transform3D::create(world.engine().ecs(), entity());
    Outline3D::init(world.engine().ecs(), entity(), &component());
    component().create(world.engine(), vc, ic);
}

void IndexBuffer3D::commit() {
    rc::prim::Vertex3D::computeTBN(
        &component().getVertex(0), &component().getIndex(0), component().getIndexCount());
    component().commit();
}

void IndexBuffer3D::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace gfx
} // namespace bl
