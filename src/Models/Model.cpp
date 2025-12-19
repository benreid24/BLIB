#include <BLIB/Models/Model.hpp>

namespace bl
{
namespace mdl
{
void Model::populate(const aiScene* scene, const std::string& path) {
    bones.populate(scene);
    nodes.reserveSpace(scene);
    nodes.addNode(0).populate(nodes, scene, scene->mRootNode, bones);
    materials.populate(scene, path);
    meshes.populate(scene, bones);
    animations.populate(scene);
    flipUVs();
}

void Model::flipUVs() { meshes.flipUVs(); }

} // namespace mdl
} // namespace bl
