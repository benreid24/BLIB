#include <BLIB/Graphics/VertexBuffer3D.hpp>

namespace bl
{
namespace gfx
{
void VertexBuffer3D::create(engine::World& world, unsigned int vc) {
    Drawable::createWithMaterial(world, rc::Config::MaterialPipelineIds::Mesh3D);
    Transform3D::create(world.engine().ecs(), entity());
    component().create(world.engine(), vc);
}

void VertexBuffer3D::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace gfx
} // namespace bl
