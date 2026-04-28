#include <BLIB/Assets/Detail/DependencyListBase.hpp>

#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Assets/Repository.hpp>

namespace bl
{
namespace as
{
namespace detail
{
DependencyListBase::DependencyListBase(Repository& repo, Payload& owner, std::string_view tag,
                                       LoadPolicy policy, DependencyPolicy depPolicy)
: DependencyChain(repo, owner, tag)
, policy(policy)
, depPolicy(depPolicy) {}

bool DependencyListBase::addDependency(util::UUID uuid) {
    for (const auto& dep : dependencies) {
        if (dep.uuid == uuid) {
            BL_LOG_WARN << "Ignoring duplicate dependency with UUID " << uuid.toString()
                        << " for asset " << owner.getAsset().getUUID().toString();
            return true;
        }
    }

    Entry& entry = dependencies.emplace_back(Entry{uuid, Ref()});
    if (!loadEntry(entry)) {
        BL_LOG_ERROR << "Failed to load dependency with UUID " << uuid.toString() << " for asset "
                     << owner.getAsset().getUUID().toString();
        dependencies.pop_back();
        return false;
    }

    registerDependency(uuid);
    return true;
}

bool DependencyListBase::load() {
    bool success = true;
    bool any     = false;
    for (auto& entry : dependencies) {
        if (!loadEntry(entry)) { success = false; }
        else { any = true; }
    }

    return getStatus(success, any);
}

bool DependencyListBase::getStatus(bool success, bool any) const {
    switch (depPolicy) {
    case DependencyPolicy::Required:
        return success;
    case DependencyPolicy::Partial:
        return any || dependencies.empty();
    case DependencyPolicy::Optional:
        return true;
    default:
        BL_LOG_ERROR << "Invalid dependency policy: " << depPolicy;
        return false;
    }
}

void DependencyListBase::unload() {
    for (auto& entry : dependencies) {
        if (entry.dependency) { entry.dependency.release(); }
    }
}

bool DependencyListBase::matchAndLoad(const std::vector<RepoDependency>& taggedDeps) {
    const auto n = std::count_if(taggedDeps.begin(),
                                 taggedDeps.end(),
                                 [this](const RepoDependency& dep) { return dep.tag == tag; });
    dependencies.reserve(n);

    bool success = true;
    bool any     = false;
    for (const auto& dep : taggedDeps) {
        if (dep.tag != tag) { continue; }

        auto& entry = dependencies.emplace_back(Entry{dep.uuid, Ref()});
        if (!loadEntry(entry)) {
            BL_LOG_ERROR << "Failed to load dependency with UUID " << dep.uuid.toString()
                         << " for asset " << owner.getAsset().getUUID().toString();
            success = false;
        }
        else { any = true; }
    }

    return getStatus(success, any);
}

bool DependencyListBase::loadEntry(Entry& entry) {
    if (entry.dependency && entry.dependency->getState() == State::Loaded) { return true; }

    const State desiredState = policy == LoadPolicy::Eager ? State::Loaded : State::Unloaded;
    entry.dependency         = repo.getAsset(entry.uuid, desiredState);
    return entry.dependency && entry.dependency->getState() >= desiredState;
}

Ref DependencyListBase::getItem(std::size_t index) const {
    if (index >= dependencies.size()) {
        BL_LOG_ERROR << "Attempted to get dependency at index " << index << " but there are only "
                     << dependencies.size() << " dependencies for asset "
                     << owner.getAsset().getUUID().toString();
        return Ref();
    }
    return dependencies[index].dependency;
}

} // namespace detail
} // namespace as
} // namespace bl
