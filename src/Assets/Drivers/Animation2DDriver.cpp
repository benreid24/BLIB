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

bool Animation2DDriver::doCreate(as::CreateContext& ctx, Animation2DPayload& payload) {
    if (ctx.getCustomData().getPath().empty()) {
        const CreateParams* createParams = ctx.getCustomDataAsMaybe<CreateParams>();
        if (createParams && createParams->spritesheet.isValid()) {
            if (!payload.spritesheet.init(createParams->spritesheet.getAsset().getUUID())) {
                return false;
            }
            payload.frames       = createParams->frames;
            payload.loop         = createParams->loop;
            payload.centerShards = createParams->centerShards;
            payload.computeDerivedData();
            return true;
        }

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

    try {
        serial::binary::detail::InputStreamWrapper input(stream);

        payload.frames.clear();

        const std::string path = util::FileUtil::getPath(ctx.getCustomData().getPath());
        const std::string& spritesheetDir =
            engine::Configuration::get<std::string>("blib.animation.spritesheet_path");

        std::string sheet;
        input.read(sheet);

        const auto trySheetPath =
            [ctx](const std::string& path) -> as::TypedRef<asi::ImagePayload> {
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
                    BL_LOG_ERROR << "Failed to resolve spritesheet reference for Animation2D asset "
                                    "from path: "
                                 << ctx.getCustomData().getPath() << " with sheet name: " << sheet;
                    return false;
                }
            }
        }

        // this should always pass here but still check
        if (!payload.spritesheet.init(resolvedSheetRef.getAsset().getUUID())) { return false; }

        input.read(payload.loop);

        std::uint16_t nFrames = 0;
        input.read(nFrames);
        payload.frames.reserve(nFrames);
        for (unsigned int i = 0; i < nFrames; ++i) {
            Animation2DPayload::Frame& frame = payload.frames.emplace_back();

            std::uint32_t length;
            input.read(length);
            frame.length = static_cast<float>(length) / 1000.0f;

            std::uint16_t nShards = 0;
            input.read(nShards);
            frame.shards.reserve(nShards);

            for (unsigned int j = 0; j < nShards; ++j) {
                frame.shards.emplace_back();
                Animation2DPayload::Frame::Shard& shard = frame.shards.back();

                uint32_t u32;
                int32_t s32;
                input.read(u32);
                shard.source.position.x = u32;
                input.read(u32);
                shard.source.position.y = u32;
                input.read(u32);
                shard.source.size.x = u32;
                input.read(u32);
                shard.source.size.y = u32;
                input.read(u32);
                shard.scale.x = static_cast<float>(u32) / 100.0f;
                input.read(u32);
                shard.scale.y = static_cast<float>(u32) / 100.0f;
                input.read(s32);
                shard.offset.x = s32;
                input.read(s32);
                shard.offset.y = s32;
                input.read(u32);
                shard.rotation = u32;
                input.read(shard.alpha);
                frame.shards.push_back(shard);
            }
        }

        // bool value not present in legacy files
        try {
            std::uint8_t u8 = 0;
            input.read(u8);
            payload.centerShards = u8 == 1;
        } catch (const serial::SerializationError&) { payload.centerShards = false; }
    } catch (const serial::SerializationError& e) {
        BL_LOG_ERROR << "Failed to deserialize Animation2D asset from path: "
                     << ctx.getCustomData().getPath() << " with error: " << e.what();
        return false;
    }

    payload.computeDerivedData();

    return true;
}

bool Animation2DDriver::doRead(as::ReadContext& ctx, Animation2DPayload& payload) {
    stream::InputStream stream;
    if (!ctx.setupReadStream("animation.banim", stream)) {
        BL_LOG_ERROR << "Failed to setup read stream for Animation2D asset: " << ctx.getAsset();
        return false;
    }
    if (!serial::binary::SerializerChecked<Animation2DPayload>::deserialize(stream, payload)) {
        BL_LOG_ERROR << "Failed to deserialize Animation2D asset: " << ctx.getAsset();
        return false;
    }
    payload.computeDerivedData();
    return true;
}

bool Animation2DDriver::doWrite(as::WriteContext& ctx, const Animation2DPayload& payload) {
    stream::OutputStream stream;
    if (!ctx.setupWriteStream("animation.banim", stream)) {
        BL_LOG_ERROR << "Failed to setup write stream for Animation2D asset: " << ctx.getAsset();
        return false;
    }
    if (!serial::binary::SerializerChecked<Animation2DPayload>::serialize(stream, payload)) {
        BL_LOG_ERROR << "Failed to serialize Animation2D asset: " << ctx.getAsset();
        return false;
    }
    return true;
}

} // namespace asi
} // namespace bl
