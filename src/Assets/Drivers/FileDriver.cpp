#include <BLIB/Assets/Drivers/FileDriver.hpp>

#include <BLIB/Util/FileUtil.hpp>

namespace bl
{
namespace asi
{
FileDriver::FileDriver()
: Driver(
      as::bdl::AssetBundleConfig{.affinity  = as::bdl::AssetBundleConfig::Affinity::Parent,
                                 .selection = as::bdl::AssetBundleConfig::Selection::NonRoot,
                                 .onMount = as::bdl::AssetBundleConfig::OnMount::WhenRequested}) {}

bool FileDriver::doCreate(as::CreateContext& ctx, FilePayload& payload) {
    if (!ctx.getCustomData().getPath().empty()) {
        return util::FileUtil::readFile(ctx.getCustomData().getPath(), payload.getData());
    }
    else {
        BL_LOG_ERROR << "FileDriver requires a file path as custom create data";
        return false;
    }
}

bool FileDriver::doRead(as::ReadContext& ctx, FilePayload& payload) {
    stream::InputStream input;
    if (!ctx.setupReadStream("file", input)) { return false; }
    return input.read(payload.getData(), input.getSize()) == input.getSize();
}

bool FileDriver::doWrite(as::WriteContext& ctx, const FilePayload& payload) {
    stream::OutputStream output;
    if (!ctx.setupWriteStream("file", output)) { return false; }
    return output.write(payload.getData().data(), payload.getData().size());
}

} // namespace asi
} // namespace bl
