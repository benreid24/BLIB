#include <BLIB/Models/MeshSet.hpp>

namespace bl
{
namespace mdl
{
void MeshSet::populate(const aiScene* scene, BoneSet& bones) {
    meshes.resize(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        meshes[i].populate(scene->mMeshes[i], bones);
    }
}

void MeshSet::flipUVs() {
    for (Mesh& mesh : meshes) { mesh.flipUVs(); }
}

Mesh& MeshSet::addMesh() { return meshes.emplace_back(); }

} // namespace mdl
} // namespace bl
