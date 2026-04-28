#include <BLIB/Assets/Detail/DependencySingleBase.hpp>

#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Assets/Repository.hpp>

namespace bl
{
namespace as
{
namespace detail
{
DependencySingleBase::DependencySingleBase(Repository& repo, Payload& owner, std::string_view tag,
                                           LoadPolicy policy, DependencyPolicy depPolicy)
: DependencyChain(repo, owner, tag)
, policy(policy)
, depPolicy(depPolicy) {}

State DependencySingleBase::getState() const {
    if (!dependency) { return State::Unknown; }
    return dependency.getState();
}

util::UUID DependencySingleBase::getUUID() const { return uuid; }

bool DependencySingleBase::init(util::UUID uuid) {
    if (uuid == util::UUID()) {
        BL_LOG_WARN << "Ignoring attempt to initialize dependency with empty UUID for asset "
                    << owner.getAsset().getUUID().toString() << " and tag '" << tag << "'";
        return true;
    }
    if (this->uuid != util::UUID()) {
        BL_LOG_ERROR << "Attempted to initialize dependency with UUID " << uuid.toString()
                     << " but it is already initialized with UUID " << this->uuid.toString();
        throw std::runtime_error("Dependency is already initialized");
    }
    this->uuid = uuid;
    registerDependency(uuid);
    return load();
}

bool DependencySingleBase::load() {
    if (dependency && dependency->getState() == State::Loaded) { return true; }
    if (uuid == util::UUID()) { return true; }
    dependency = repo.getAsset(uuid, State::Loaded);
    return dependency && dependency->getState() == State::Loaded;
}

void DependencySingleBase::unload() {
    if (dependency) { dependency.release(); }
}

bool DependencySingleBase::matchAndLoad(const std::vector<RepoDependency>& dependencies) {
    const RepoDependency* match = nullptr;
    for (const auto& dep : dependencies) {
        if (dep.tag == tag) {
            if (!match) { match = &dep; }
            else {
                BL_LOG_ERROR << "Multiple dependencies with tag '" << tag << "' found for asset "
                             << owner.getAsset().getUUID().toString();
            }
        }
    }
    if (!match) {
        BL_LOG_ERROR << "Failed to find dependency with tag '" << tag << "' for asset "
                     << owner.getAsset().getUUID().toString();
        return depPolicy != DependencyPolicy::Required;
    }
    uuid = match->uuid;
    if (policy == LoadPolicy::Eager) {
        if (!load()) {
            BL_LOG_ERROR << "Failed to load dependency with UUID " << uuid.toString()
                         << " for asset " << owner.getAsset().getUUID().toString();
            return depPolicy != DependencyPolicy::Required;
        }
    }
    return true;
}

} // namespace detail
} // namespace as
} // namespace bl
