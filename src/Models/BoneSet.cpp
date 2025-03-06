#include <BLIB/Models/BoneSet.hpp>

namespace bl
{
namespace mdl
{
void BoneSet::populate(const aiScene* scene) {
    // TODO - we may want to use skeletons instead for more complex animations + blending

    unsigned int boneCount = 0;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        boneCount += scene->mMeshes[i]->mNumBones;
    }

    bones.reserve(boneCount);
    boneSources.reserve(boneCount);

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        for (unsigned int j = 0; j < scene->mMeshes[i]->mNumBones; ++j) {
            getOrAddBone(scene->mMeshes[i]->mBones[j]);
        }
    }
}

unsigned int BoneSet::getOrAddBone(const aiBone* bone) {
    for (unsigned int i = 0; i < boneSources.size(); ++i) {
        if (boneSources[i] == bone) { return i; }
    }

    const unsigned int i = bones.size();
    bones.emplace_back(bone);
    boneSources.emplace_back(bone);
    return i;
}

} // namespace mdl
} // namespace bl
