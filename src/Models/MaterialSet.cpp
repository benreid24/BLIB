#include <BLIB/Models/MaterialSet.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace mdl
{
void MaterialSet::populate(const aiScene* scene, const std::string& modelPath) {
    materials.reserve(scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        const aiMaterial* src = scene->mMaterials[i];
        Material& material    = materials.emplace_back();

        const auto loadTexture = [scene, src, &modelPath](aiTextureType type, Texture& tex) {
            aiString path;
            if (src->GetTexture(type, 0, &path) == aiReturn_SUCCESS) {
                const aiTexture* texture = scene->GetEmbeddedTexture(path.C_Str());
                if (texture) {
                    tex.makeFromRaw(texture->mWidth, texture->mHeight, texture->pcData);
                }
                else { tex.makeFromFile(path.C_Str(), modelPath); }
            }
        };

        for (int mkey = aiTextureType_DIFFUSE; mkey < AI_TEXTURE_TYPE_MAX; ++mkey) {
            aiString path;
            aiTextureType type = aiTextureType(mkey);
            if (src->GetTexture(type, 0, &path) == aiReturn_SUCCESS) {
                BL_LOG_DEBUG << "Found texture of type " << type << " at path: " << path.C_Str();
            }
        }

        src->Get(AI_MATKEY_BUMPSCALING, material.heightScale);
        src->Get(AI_MATKEY_SHININESS, material.shininess);
        loadTexture(aiTextureType_DIFFUSE, material.diffuse);
        loadTexture(aiTextureType_NORMALS, material.normal);
        loadTexture(aiTextureType_SPECULAR, material.specular);
        loadTexture(aiTextureType_HEIGHT, material.parallax);
    }
}

} // namespace mdl
} // namespace bl
