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
}

void Model::mergeChildren() {
    nodes.getNode(0).mergeChildren(nodes);
    nodes.clearAllButRoot();
}

} // namespace mdl
} // namespace bl
