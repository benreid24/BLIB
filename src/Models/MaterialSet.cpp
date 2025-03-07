#include <BLIB/Models/MaterialSet.hpp>

namespace bl
{
namespace mdl
{
void MaterialSet::populate(const aiScene* scene) {
    materials.reserve(scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        const aiMaterial* src = scene->mMaterials[i];

        const auto loadTexture = [scene, src](aiTextureType type, Texture& tex) {
            aiString path;
            if (src->GetTexture(type, 0, &path) == aiReturn_SUCCESS) {
                const aiTexture* texture = scene->GetEmbeddedTexture(path.C_Str());
                if (texture) {
                    tex.makeFromRaw(texture->mWidth, texture->mHeight, texture->pcData);
                }
                else { tex.makeFromFile(path.C_Str()); }
            }
        };

        src->Get(AI_MATKEY_BUMPSCALING, materials[i].heightScale);
        src->Get(AI_MATKEY_SHININESS, materials[i].shininess);
        loadTexture(aiTextureType_DIFFUSE, materials[i].diffuse);
        loadTexture(aiTextureType_NORMALS, materials[i].normal);
        loadTexture(aiTextureType_SPECULAR, materials[i].specular);
        loadTexture(aiTextureType_HEIGHT, materials[i].parallax);
    }
}

} // namespace mdl
} // namespace bl
