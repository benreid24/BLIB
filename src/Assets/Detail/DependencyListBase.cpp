#include <BLIB/Assets/Detail/DependencyListBase.hpp>

#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Assets/Repository.hpp>

namespace bl
{
namespace as
{
namespace detail
{
DependencyListBase::DependencyListBase(Repository& repo, Payload& owner, std::string_view tag)
: DependencyChain(repo, owner, tag) {}

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
    for (auto& entry : dependencies) {
        if (!loadEntry(entry)) { success = false; }
    }
    return success;
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
    for (const auto& dep : taggedDeps) {
        if (dep.tag != tag) { continue; }

        auto& entry = dependencies.emplace_back(Entry{dep.uuid, Ref()});
        if (!loadEntry(entry)) {
            BL_LOG_ERROR << "Failed to load dependency with UUID " << dep.uuid.toString()
                         << " for asset " << owner.getAsset().getUUID().toString();
            success = false;
        }
    }

    return success;
}

bool DependencyListBase::loadEntry(Entry& entry) {
    if (entry.dependency && entry.dependency->getState() == State::Loaded) { return true; }
    entry.dependency = repo.getAsset(entry.uuid, State::Loaded);
    return entry.dependency && entry.dependency->getState() == State::Loaded;
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
