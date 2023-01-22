#include <BLIB/Render/Renderables/3D/Mesh.hpp>

namespace bl
{
namespace render
{
namespace r3d
{
Mesh::Mesh() {
    addOrSetPassPipeline(Config::RenderPassIds::Opaque, Config::PipelineIds::OpaqueMeshes);
}

void Mesh::attachBuffer() { setDrawParameters(indices.getDrawParameters()); }

} // namespace r3d
} // namespace render
} // namespace bl
