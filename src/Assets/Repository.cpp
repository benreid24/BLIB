#include <BLIB/Assets/Repository.hpp>

namespace bl
{
namespace as
{
Repository::Repository()
: mode(Mode::Editor) {
    drivers.reserve(16);
}

Ref Repository::getAsset(util::UUID uuid, State desiredState) {
    std::unique_lock lock(mutex);
    auto it = assets.find(uuid);
    if (it == assets.end()) {
        BL_LOG_ERROR << "Attempted to get asset with UUID " << uuid.toString()
                     << " but it does not exist";
        return Ref();
    }
    StoredAsset& stored = it->second;
    if (!stored.asset) {
        // TODO - load metadata for the asset? maybe make non-optional instead
    }
    if (stored.asset->getState() < desiredState) {
        // TODO - attempt to load the asset
    }
    // TODO - implement ref counting
}

} // namespace as
} // namespace bl
