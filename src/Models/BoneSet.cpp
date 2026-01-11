#include <BLIB/Models/BoneSet.hpp>

namespace bl
{
namespace mdl
{
void BoneSet::populate(const aiScene* scene) {
    unsigned int boneCount = 0;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        boneCount += scene->mMeshes[i]->mNumBones;
    }

    bones.reserve(boneCount);

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        for (unsigned int j = 0; j < scene->mMeshes[i]->mNumBones; ++j) {
            getOrAddBone(scene->mMeshes[i]->mBones[j]);
        }
    }
}

unsigned int BoneSet::getOrAddBone(const aiBone* bone) {
    const auto existingIndex =
        getBoneIndexByName(std::string_view(bone->mName.data, bone->mName.length));
    if (existingIndex.has_value()) { return existingIndex.value(); }

    const unsigned int i = bones.size();
    bones.emplace_back(bone);
    return i;
}

std::optional<unsigned int> BoneSet::getBoneIndexByName(const std::string_view& name) const {
    for (unsigned int i = 0; i < bones.size(); ++i) {
        if (bones[i].name == name) { return i; }
    }
    return std::nullopt;
}

} // namespace mdl
} // namespace bl
