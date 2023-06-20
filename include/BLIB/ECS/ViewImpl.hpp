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
View<TComponents...>::View(Registry& reg, std::size_t ec, ComponentMask::Value mask)
: ViewBase(mask)
, registry(reg)
, pools({&reg.getPool<TComponents>()...})
, containedEntities(ec, false) {
    results.reserve(ec / 2);
    reg.populateViewWithLock(*this);
    toAdd.reserve(32);
    toRemove.reserve(32);
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

    for (Entity ent : toAdd) {
        if (containedEntities[ent]) { continue; }
        results.emplace_back(registry, ent);
        if (!results.back().isValid()) { results.pop_back(); }
        else { containedEntities[ent] = true; }
    }
    toAdd.clear();

    for (Entity ent : toRemove) {
        containedEntities[ent] = false;
        for (auto it = results.begin(); it != results.end(); ++it) {
            if (it->entity() == ent) {
                results.erase(it);
                break;
            }
        }
    }
    toRemove.clear();

    unlockWrite();
}

template<typename... TComponents>
void View<TComponents...>::clearAndRefresh() {
    lockWrite();
    results.clear();
    for (auto it = containedEntities.begin(); it != containedEntities.end(); ++it) { *it = false; }
    registry.populateView(*this);
    unlockWrite();
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
