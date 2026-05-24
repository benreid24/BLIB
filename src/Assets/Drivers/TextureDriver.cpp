#include <BLIB/Assets/Drivers/TextureDriver.hpp>

namespace bl
{
namespace asi
{
TextureDriver::TextureDriver()
: Driver(
      as::bdl::AssetBundleConfig{.affinity  = as::bdl::AssetBundleConfig::Affinity::Parent,
                                 .selection = as::bdl::AssetBundleConfig::Selection::NonRoot,
                                 .onMount = as::bdl::AssetBundleConfig::OnMount::WhenRequested}) {}

bool TextureDriver::doCreate(const as::CreateContext& ctx, TexturePayload& payload) {
    const TexturePayload::CreateData* createData =
        ctx.getCustomDataAsMaybe<TexturePayload::CreateData>();
    if (createData) {
        payload.type       = createData->type;
        payload.colorSpace = createData->colorSpace;
        if (createData->image != util::UUID()) { return payload.image.init(createData->image); }
    }

    if (ctx.getCustomData().getPath().empty()) { return false; }

    auto image =
        ctx.getRepository().getAssetFromSourcePath<ImagePayload>(ctx.getCustomData().getPath());
    if (!image) { return false; }
    return payload.image.init(image.getAsset().getUUID());
}

bool TextureDriver::doRead(const as::ReadContext& ctx, TexturePayload& payload) {
    stream::InputStream input;
    if (!ctx.setupReadStream("textureMetadata.json", input)) { return false; }
    if (ctx.getMode() == as::Mode::Editor) {
        return serial::json::Serializer<TexturePayload>::deserializeStream(input, payload);
    }
    else { return serial::binary::Serializer<TexturePayload>::deserialize(input, payload); }
}

bool TextureDriver::doWrite(const as::WriteContext& ctx, const TexturePayload& payload) {
    stream::OutputStream output;
    if (!ctx.setupWriteStream("textureMetadata.json", output)) { return false; }
    if (ctx.getMode() == as::Mode::Editor) {
        return serial::json::Serializer<TexturePayload>::serializeStream(output, payload, 4, 0);
    }
    else { return serial::binary::Serializer<TexturePayload>::serialize(output, payload); }
}

} // namespace asi
} // namespace bl
