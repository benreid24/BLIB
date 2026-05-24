#include <BLIB/Assets/Drivers/Animation2DDriver.hpp>

#include <BLIB/Assets/Drivers/Animation2DSetDriver.hpp>
#include <BLIB/Engine/Configuration.hpp>

namespace bl
{
namespace asi
{
Animation2DDriver::Animation2DDriver()
: Driver(
      as::bdl::AssetBundleConfig{.affinity  = as::bdl::AssetBundleConfig::Affinity::Parent,
                                 .selection = as::bdl::AssetBundleConfig::Selection::NonRoot,
                                 .onMount = as::bdl::AssetBundleConfig::OnMount::WhenRequested}) {}

bool Animation2DDriver::doCreate(const as::CreateContext& ctx, Animation2DPayload& payload) {
    if (ctx.getCustomData().getPath().empty()) {
        const Animation2DSetPayload::CreateData* debugData =
            ctx.getCustomDataAsMaybe<Animation2DSetPayload::CreateData>();
        if (debugData && debugData->debugCreateData.has_value()) {
            const auto& debugInit = debugData->debugCreateData.value();
            if (!payload.spritesheet.init(debugInit.spritesheet.getUUID())) { return false; }
            payload.frames       = std::move(debugInit.frames);
            payload.loop         = debugInit.loop;
            payload.centerShards = debugInit.centerShards;
            payload.computeDerivedData();
            return true;
        }
        else { BL_LOG_ERROR << "No path provided for Animation2D asset creation"; }
        return false;
    }

    stream::InputStream stream;
    if (!stream.open(ctx.getCustomData().getPath())) {
        BL_LOG_ERROR << "Failed to open stream for Animation2D asset creation from path: "
                     << ctx.getCustomData().getPath();
        return false;
    }

    serial::binary::detail::InputStreamWrapper input(stream);

    payload.frames.clear();

    const std::string path = util::FileUtil::getPath(ctx.getCustomData().getPath());
    const std::string& spritesheetDir =
        engine::Configuration::get<std::string>("blib.animation.spritesheet_path");

    std::string sheet;
    if (!input.read(sheet)) return false;

    const auto trySheetPath = [ctx](const std::string& path) -> as::TypedRef<asi::ImagePayload> {
        if (util::FileUtil::exists(path)) {
            return ctx.getRepository().getAssetFromSourcePath<ImagePayload>(path);
        }
        return {};
    };

    // 1. first try whole past as is
    as::Ref resolvedSheetRef = trySheetPath(sheet);
    if (!resolvedSheetRef) {
        // 2. then try relative to the animation file
        resolvedSheetRef = trySheetPath(util::FileUtil::joinPath(path, sheet));
        if (!resolvedSheetRef) {
            // 3. then try relative to the spritesheet directory
            resolvedSheetRef = trySheetPath(util::FileUtil::joinPath(spritesheetDir, sheet));
            if (!resolvedSheetRef) {
                // 4. give up
                BL_LOG_ERROR
                    << "Failed to resolve spritesheet reference for Animation2D asset from path: "
                    << ctx.getCustomData().getPath() << " with sheet name: " << sheet;
                return false;
            }
        }
    }

    // this should always pass here but still check
    if (!payload.spritesheet.init(resolvedSheetRef.getAsset().getUUID())) { return false; }

    if (!input.read(payload.loop)) return false;

    std::uint16_t nFrames = 0;
    if (!input.read(nFrames)) return false;
    payload.frames.reserve(nFrames);
    for (unsigned int i = 0; i < nFrames; ++i) {
        Animation2DPayload::Frame& frame = payload.frames.emplace_back();

        std::uint32_t length;
        if (!input.read(length)) return false;
        frame.length = static_cast<float>(length) / 1000.0f;

        std::uint16_t nShards = 0;
        if (!input.read(nShards)) return false;
        frame.shards.reserve(nShards);

        for (unsigned int j = 0; j < nShards; ++j) {
            frame.shards.emplace_back();
            Animation2DPayload::Frame::Shard& shard = frame.shards.back();

            uint32_t u32;
            int32_t s32;
            if (!input.read(u32)) return false;
            shard.source.position.x = u32;
            if (!input.read(u32)) return false;
            shard.source.position.y = u32;
            if (!input.read(u32)) return false;
            shard.source.size.x = u32;
            if (!input.read(u32)) return false;
            shard.source.size.y = u32;
            if (!input.read(u32)) return false;
            shard.scale.x = static_cast<float>(u32) / 100.0f;
            if (!input.read(u32)) return false;
            shard.scale.y = static_cast<float>(u32) / 100.0f;
            if (!input.read(s32)) return false;
            shard.offset.x = s32;
            if (!input.read(s32)) return false;
            shard.offset.y = s32;
            if (!input.read(u32)) return false;
            shard.rotation = u32;
            if (!input.read(shard.alpha)) return false;
            frame.shards.push_back(shard);
        }
    }

    // bool value not present in legacy files
    std::uint8_t u8;
    if (!input.read(u8)) { payload.centerShards = false; }
    else { payload.centerShards = u8 == 1; }

    return true;
}

bool Animation2DDriver::doRead(const as::ReadContext& ctx, Animation2DPayload& payload) {
    stream::InputStream stream;
    if (!ctx.setupReadStream("animation.banim", stream)) {
        BL_LOG_ERROR << "Failed to setup read stream for Animation2D asset with UUID: "
                     << ctx.getAsset().getUUID();
        return false;
    }
    if (!serial::binary::Serializer<Animation2DPayload>::deserialize(stream, payload)) {
        BL_LOG_ERROR << "Failed to deserialize Animation2D asset with UUID: "
                     << ctx.getAsset().getUUID();
        return false;
    }
    return true;
}

bool Animation2DDriver::doWrite(const as::WriteContext& ctx, const Animation2DPayload& payload) {
    stream::OutputStream stream;
    if (!ctx.setupWriteStream("animation.banim", stream)) {
        BL_LOG_ERROR << "Failed to setup write stream for Animation2D asset with UUID: "
                     << ctx.getAsset().getUUID();
        return false;
    }
    if (!serial::binary::Serializer<Animation2DPayload>::serialize(stream, payload)) {
        BL_LOG_ERROR << "Failed to serialize Animation2D asset with UUID: "
                     << ctx.getAsset().getUUID();
        return false;
    }
    return true;
}

} // namespace asi
} // namespace bl
