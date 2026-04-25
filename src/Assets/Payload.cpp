#include <BLIB/Assets/Payload.hpp>

#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace as
{
namespace detail
{
DependencyChain::DependencyChain(Repository& repo, Payload& owner, std::string_view tag)
: tag(tag)
, repo(repo)
, owner(owner)
, next(nullptr)
, loaded(false) {
    owner.registerDependency(this);
}

State DependencyChain::getState() const {
    if (!dependency) { return State::Unknown; }
    return dependency.getState();
}

util::UUID DependencyChain::getUUID() const { return uuid; }

bool DependencyChain::init(util::UUID uuid) {
    if (this->uuid != util::UUID()) {
        BL_LOG_ERROR << "Attempted to initialize dependency with UUID " << uuid.toString()
                     << " but it is already initialized with UUID " << this->uuid.toString();
        throw std::runtime_error("Dependency is already initialized");
    }
    this->uuid = uuid;
    repo.registerDependency(owner.getAsset().getUUID(), tag, uuid);
    return load();
}

bool DependencyChain::load() {
    if (dependency && dependency->getState() == State::Loaded) { return true; }
    if (uuid == util::UUID()) { true; }
    dependency = repo.getAsset(uuid, State::Loaded);
    return dependency && dependency->getState() == State::Loaded;
}

void DependencyChain::unload() {
    if (dependency) { dependency.release(); }
}

} // namespace detail

Payload::Payload(const ConstructContext& ctx)
: repo(ctx.repo)
, owner(ctx.asset)
, dependencyChain(nullptr) {}

void Payload::registerDependency(detail::DependencyChain* chain) {
    if (dependencyChain) {
        detail::DependencyChain* current = dependencyChain;
        while (current->next) { current = current->next; }
        current->next = chain;
    }
    else { dependencyChain = chain; }
}

bool Payload::loadDependencies() {
    const auto findDep = [this](std::string_view tag) -> detail::DependencyChain* {
        detail::DependencyChain* current = dependencyChain;
        while (current) {
            if (current->tag == tag) { return current; }
            current = current->next;
        }
        return nullptr;
    };

    bool success            = true;
    const auto dependencies = repo.getDependencies(owner.getUUID());
    for (const auto& depInfo : dependencies) {
        detail::DependencyChain* dep = findDep(depInfo.tag);
        if (!dep) {
            BL_LOG_WARN << "Asset " << owner.getUUID().toString() << " has dependency with tag '"
                        << depInfo.tag << "' but it was not registered by the payload";
            continue;
        }
        if (!dep->init(depInfo.uuid)) {
            BL_LOG_ERROR << "Failed to load dependency " << depInfo.uuid.toString() << " for asset "
                         << owner.getUUID().toString() << " with tag '" << depInfo.tag << "'";
            dep->loaded = true;
            success     = false;
        }
    }
    if (!success) { return false; }

    // check all deps loaded
    bool allLoaded                   = true;
    detail::DependencyChain* current = dependencyChain;
    while (current) {
        if (!current->loaded) {
            allLoaded = false;
            BL_LOG_ERROR << "Dependency " << current->uuid.toString() << " for asset "
                         << owner.getUUID().toString() << " with tag '" << current->tag
                         << "' was not found";
        }
        current = current->next;
    }
    return allLoaded;
}

void Payload::flush() { owner.flushPayload(); }

} // namespace as
} // namespace bl
