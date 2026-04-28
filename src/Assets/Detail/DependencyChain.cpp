#include <BLIB/Assets/Detail/DependencyChain.hpp>

#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Assets/Repository.hpp>

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
, next(nullptr) {
    owner.registerDependency(this);
}

void DependencyChain::registerDependency(util::UUID uuid) {
    repo.registerDependency(owner.getAsset().getUUID(), tag, uuid);
}

bool DependencyChain::unregisterDependency(util::UUID uuid) {
    return repo.unregisterDependency(owner.getAsset().getUUID(), tag, uuid);
}

} // namespace detail
} // namespace as
} // namespace bl
