#include <BLIB/Assets/Drivers/FontDriver.hpp>

namespace bl
{
namespace asi
{
FontDriver::CreateParams::CreateParams(const std::string& path)
: CreateData(path)
, data() {}

FontDriver::CreateParams::CreateParams(std::span<const char> data)
: CreateData()
, data(data) {}

FontDriver::FontDriver()
: Driver(as::bdl::AssetBundleConfig{.affinity  = as::bdl::AssetBundleConfig::Affinity::Type,
                                    .selection = as::bdl::AssetBundleConfig::Selection::Root,
                                    .onMount   = as::bdl::AssetBundleConfig::OnMount::AutoLoad}) {}

bool FontDriver::doCreate(as::CreateContext& ctx, FontPayload& payload) {
    if (const CreateParams* params = ctx.getCustomDataAsMaybe<CreateParams>()) {
        if (params->data.size() > 0) {
            payload.buffer.resize(params->data.size());
            std::memcpy(payload.buffer.data(), params->data.data(), params->data.size());
        }
    }
    else if (ctx.getCustomData().getPath().empty()) {
        if (!util::FileUtil::readFile(ctx.getCustomData().getPath(), payload.buffer)) {
            BL_LOG_ERROR << "Failed to read font file from " << ctx.getCustomData().getPath();
            return false;
        }
    }
    else {
        BL_LOG_ERROR << "No path or create params provided for font asset creation";
        return false;
    }

    return payload.loadFromMemory();
}

bool FontDriver::doRead(as::ReadContext& ctx, FontPayload& payload) {
    stream::InputStream input;
    if (!ctx.setupReadStream("font.ttf", input)) { return false; }
    if (input.read(payload.buffer, input.getSize()) != input.getSize()) { return false; }
    return payload.loadFromMemory();
}

bool FontDriver::doWrite(as::WriteContext& ctx, const FontPayload& payload) {
    stream::OutputStream output;
    if (!ctx.setupWriteStream("font.ttf", output)) { return false; }
    return output.write(payload.buffer.data(), payload.buffer.size());
}

} // namespace asi
} // namespace bl
