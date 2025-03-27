#include <BLIB/Models/Model.hpp>

namespace bl
{
namespace mdl
{
void Model::populate(const aiScene* scene, const std::string& path) {
    root.populate(scene, scene->mRootNode, bones);
    materials.populate(scene, path);
    bones.populate(scene);
}

void Model::mergeChildren() { root.mergeChildren(); }

} // namespace mdl
} // namespace bl
