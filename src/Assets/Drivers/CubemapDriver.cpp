#include <BLIB/Assets/Drivers/CubemapDriver.hpp>

namespace bl
{
namespace asi
{
CubemapDriver::CubemapDriver()
: Driver(
      as::bdl::AssetBundleConfig{.affinity  = as::bdl::AssetBundleConfig::Affinity::Parent,
                                 .selection = as::bdl::AssetBundleConfig::Selection::NonRoot,
                                 .onMount = as::bdl::AssetBundleConfig::OnMount::WhenRequested}) {}

bool CubemapDriver::doCreate(const as::CreateContext& ctx, CubemapPayload& payload) {
    const CubemapPayload::CreateParams* params =
        ctx.getCustomDataAsMaybe<CubemapPayload::CreateParams>();
    if (!params) {
        BL_LOG_ERROR << "No create params provided for cubemap asset creation";
        return false;
    }
    bool success = payload.top.init(params->top);
    success      = success && payload.bottom.init(params->bottom);
    success      = success && payload.left.init(params->left);
    success      = success && payload.right.init(params->right);
    success      = success && payload.front.init(params->front);
    success      = success && payload.back.init(params->back);
    if (!success) {
        BL_LOG_ERROR << "Failed to initialize dependencies for cubemap asset creation";
        return false;
    }
    return true;
}

bool CubemapDriver::doRead(const as::ReadContext&, CubemapPayload&) {
    // dependencies are handled automatically
    return true;
}

bool CubemapDriver::doWrite(const as::WriteContext&, const CubemapPayload&) {
    // nothing to write, all data is in the dependencies
    return true;
}

} // namespace asi
} // namespace bl
