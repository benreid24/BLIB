#ifndef BLIB_ECS_VIEWIMPL_HPP
#define BLIB_ECS_VIEWIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

namespace bl
{
namespace ecs
{
template<typename... TComponents>
View<TComponents...>::View(Registry& reg, ComponentMask::Value mask)
: ViewBase(mask)
, registry(reg)
, pools({&reg.getPool<TComponents>()...}) {
    results.reserve(256);
    entityToIndex.resize(256, InvalidIndex);
    toAdd.reserve(128);
    toRemove.reserve(128);
    reg.populateViewWithLock(*this);
    ensureUpdated();
}

template<typename... TComponents>
template<typename TCallback>
void View<TComponents...>::forEach(const TCallback& cb) {
    static_assert(std::is_invocable<TCallback, ComponentSet<TComponents...>&>::value,
                  "visitor must have signature void(ComponentSet<TComponents...>&)");

    ensureUpdated();

    viewLock.lockRead();
    lockPoolsRead();
    for (auto& set : results) { cb(set); }
    unlockPoolsRead();
    viewLock.unlockRead();
}

template<typename... TComponents>
void View<TComponents...>::removeEntity(Entity entity) {
    std::unique_lock lock(queueLock);
    toRemove.emplace_back(entity);
}

template<typename... TComponents>
void View<TComponents...>::tryAddEntity(Entity ent) {
    std::unique_lock lock(queueLock);
    toAdd.emplace_back(ent);
}

template<typename... TComponents>
void View<TComponents...>::ensureUpdated() {
    std::unique_lock lock(queueLock);
    lockWrite();

    if (needsAddressReload) {
        needsAddressReload = false;
        for (auto& set : results) { set.refresh(registry); }
    }

    for (Entity ent : toAdd) {
        if (ent + 1 >= entityToIndex.size()) { entityToIndex.resize(ent + 1, InvalidIndex); }

        if (entityToIndex[ent] != InvalidIndex) { continue; }
        const std::size_t ni = results.size();
        results.emplace_back(registry, ent);
        if (!results.back().isValid()) { results.pop_back(); }
        else { entityToIndex[ent] = ni; }
    }
    toAdd.clear();

    for (Entity ent : toRemove) {
        if (ent + 1 >= entityToIndex.size()) { entityToIndex.resize(ent + 1, InvalidIndex); }

        std::size_t& index = entityToIndex[ent];
        if (index != results.size() - 1) {
            std::size_t& backIndex = entityToIndex[results.back().entity()];
            results[index]         = results.back();
            backIndex              = index;
        }
        results.pop_back();
        index = InvalidIndex;
    }
    toRemove.clear();

    unlockWrite();
}

template<typename... TComponents>
void View<TComponents...>::clearAndRefresh() {
    lockWrite();
    results.clear();
    std::fill(entityToIndex.begin(), entityToIndex.end(), InvalidIndex);
    registry.populateView(*this);
    unlockWrite();
    ensureUpdated();
}

template<typename... TComponents>
void View<TComponents...>::lockPoolsRead() {
    for (ComponentPoolBase* pool : pools) { pool->poolLock.lockRead(); }
}

template<typename... TComponents>
void View<TComponents...>::unlockPoolsRead() {
    for (ComponentPoolBase* pool : pools) { pool->poolLock.unlockRead(); }
}

template<typename... TComponents>
void View<TComponents...>::lockWrite() {
    viewLock.lockWrite();
    lockPoolsRead();
}

template<typename... TComponents>
void View<TComponents...>::unlockWrite() {
    viewLock.unlockWrite();
    unlockPoolsRead();
}

} // namespace ecs
} // namespace bl

#endif
