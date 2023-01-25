#include <BLIB/Render/Renderables/3D/Mesh.hpp>

namespace bl
{
namespace render
{
namespace r3d
{
Mesh::Mesh() {
    addOrSetStagePipeline(Config::Stage::PrimaryOpaque, Config::PipelineIds::OpaqueMeshes);
}

void Mesh::attachBuffer() { setDrawParameters(indices.getDrawParameters()); }

} // namespace r3d
} // namespace render
} // namespace bl
