#include <BLIB/Models/Model.hpp>

namespace bl
{
namespace mdl
{
void Model::populate(const aiScene* scene, const std::string& path) {
    nodes.reserveSpace(scene);
    nodes.addNode(0).populate(nodes, scene, scene->mRootNode, bones);
    materials.populate(scene, path);
    bones.populate(scene);
    meshes.populate(scene, bones);
    flipUVs();
}

void Model::flipUVs() { meshes.flipUVs(); }

} // namespace mdl
} // namespace bl
