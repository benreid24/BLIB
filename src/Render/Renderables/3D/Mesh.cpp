#include <BLIB/Render/Renderables/3D/Mesh.hpp>

namespace bl
{
namespace render
{
namespace r3d
{
Mesh::Mesh(std::uint32_t pid, bool transparent) {
    addOrSetStagePipeline(transparent ? Config::SceneObjectStage::PrimaryTransparent :
                                        Config::SceneObjectStage::PrimaryOpaque,
                          pid);
}

void Mesh::attachBuffer() { setDrawParameters(indices.getDrawParameters()); }

} // namespace r3d
} // namespace render
} // namespace bl
