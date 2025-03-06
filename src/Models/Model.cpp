#include <BLIB/Models/Model.hpp>

namespace bl
{
namespace mdl
{
void Model::populate(const aiScene* scene) {
    root.populate(scene, scene->mRootNode, bones);
    materials.populate(scene);
    bones.populate(scene);
}

} // namespace mdl
} // namespace bl
