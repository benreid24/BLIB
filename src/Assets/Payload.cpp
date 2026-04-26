#include <BLIB/Assets/Payload.hpp>

#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace as
{

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
    bool success            = true;
    const auto dependencies = repo.getDependencies(owner.getUUID());

    detail::DependencyChain* current = dependencyChain;
    while (current) {
        if (!current->matchAndLoad(dependencies)) { success = false; }
        current = current->next;
    }

    return success;
}

void Payload::flush() { owner.flushPayload(); }

} // namespace as
} // namespace bl
