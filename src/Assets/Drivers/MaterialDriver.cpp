#include <BLIB/Assets/Drivers/MaterialDriver.hpp>

#include <BLIB/Assets/Drivers/ImageDriver.hpp>

namespace bl
{
namespace asi
{
MaterialDriver::MaterialDriver()
: Driver(
      as::bdl::AssetBundleConfig{.affinity  = as::bdl::AssetBundleConfig::Affinity::Parent,
                                 .selection = as::bdl::AssetBundleConfig::Selection::NonRoot,
                                 .onMount = as::bdl::AssetBundleConfig::OnMount::WhenRequested}) {}

bool MaterialDriver::doCreate(as::CreateContext& ctx, MaterialPayload& payload) {
    const MaterialPayload::CreateData* createData =
        ctx.getCustomDataAsMaybe<MaterialPayload::CreateData>();
    if (!createData) { return false; } // allow creating empty?

    const auto prepareTexture =
        [&ctx](
            as::Dependency<TexturePayload, as::LoadPolicy::Eager, as::DependencyPolicy::Optional>&
                dst,
            const mdl::Texture& tex,
            TexturePayload::Type type) -> bool {
        if (!tex.isEmbedded() && tex.getFilePath().empty()) { return true; }

        if (tex.isEmbedded()) {
            ImagePayload::CreateData createParams;
            createParams.sourceImage = &tex.getEmbedded();

            if (!ctx.getCustomData().getIsSourceReload() || !dst.getRef().isValid()) {
                auto imageAsset = ctx.getRepository().createAsset<ImagePayload>(
                    ctx.getAsset().getMetadata().getDisplayName() + "_embeddedTexture",
                    createParams);
                if (!imageAsset) {
                    BL_LOG_ERROR << "Failed to create image asset for embedded texture in material "
                                 << ctx.getAsset();
                    return false;
                }

                TexturePayload::CreateData texCreateData;
                texCreateData.image = imageAsset->getAsset().getUUID();
                texCreateData.type  = type;
                auto textureAsset   = ctx.getRepository().createAsset<TexturePayload>(
                    ctx.getAsset().getMetadata().getDisplayName() + "_texture", texCreateData);
                if (!textureAsset) {
                    BL_LOG_ERROR
                        << "Failed to create texture asset for embedded texture in material "
                        << ctx.getAsset();
                    return false;
                }

                return dst.init(textureAsset->getAsset().getUUID());
            }
            else {
                // recreating
                dst.get().type = type;
                if (!dst.get().image.getAsset().forceRecreatePayload(createParams)) {
                    BL_LOG_ERROR << "Failed to force recreate image asset for embedded texture in "
                                    "material "
                                 << ctx.getAsset();
                    return false;
                }
                return true;
            }
        }
        else {
            auto texture =
                ctx.getRepository().getAssetFromSourcePath<TexturePayload>(tex.getFilePath());
            if (!texture) {
                BL_LOG_ERROR << "Failed to find or create texture asset for texture file "
                             << tex.getFilePath() << " in material " << ctx.getAsset();
                return false;
            }
            texture->type = type;

            if (ctx.getCustomData().getIsSourceReload()) {
                if (!texture.getAsset().reloadFromSource()) {
                    BL_LOG_ERROR << "Failed to reload texture asset for texture file "
                                 << tex.getFilePath() << " in material "
                                 << ctx.getAsset();
                    return false;
                }
            }

            texture->flush();

            return dst.init(texture->getAsset().getUUID());
        }
    };

    if (!prepareTexture(
            payload.diffuse, createData->material.diffuse, TexturePayload::Type::Diffuse)) {
        return false;
    }
    if (!prepareTexture(
            payload.normal, createData->material.normal, TexturePayload::Type::NormalMap)) {
        return false;
    }
    if (!prepareTexture(
            payload.specular, createData->material.specular, TexturePayload::Type::SpecularMap)) {
        return false;
    }
    if (!prepareTexture(payload.parallax,
                        createData->material.parallax,
                        TexturePayload::Type::DisplacementMap)) {
        return false;
    }
    payload.shininess   = createData->material.shininess;
    payload.heightScale = createData->material.heightScale;
    return true;
}

bool MaterialDriver::doRead(as::ReadContext& ctx, MaterialPayload& payload) {
    stream::InputStream input;
    if (!ctx.setupReadStream("materialMetadata.json", input)) { return false; }
    if (ctx.getMode() == as::Mode::Editor) {
        return serial::json::Serializer<MaterialPayload>::deserializeStream(input, payload);
    }
    else { return serial::binary::Serializer<MaterialPayload>::deserialize(input, payload); }
}

bool MaterialDriver::doWrite(as::WriteContext& ctx, const MaterialPayload& payload) {
    stream::OutputStream output;
    if (!ctx.setupWriteStream("materialMetadata.json", output)) { return false; }
    if (ctx.getMode() == as::Mode::Editor) {
        return serial::json::Serializer<MaterialPayload>::serializeStream(output, payload, 4, 0);
    }
    else { return serial::binary::Serializer<MaterialPayload>::serialize(output, payload); }
}

} // namespace asi
} // namespace bl
