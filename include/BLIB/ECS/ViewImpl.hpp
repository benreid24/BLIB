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
, pools({&reg.getPool<TComponents>()...})
, containedEntities(ec, false) {
    results.reserve(ec / 2);
    reg.populateViewWithLock(*this);
}

template<typename... TComponents>
template<typename TCallback>
void View<TComponents...>::forEach(const TCallback& cb) {
    static_assert(std::is_invocable<TCallback, ComponentSet<TComponents...>&>::value,
                  "visitor must have signature void(ComponentSet<TComponents...>&)");

    viewLock.lockRead();
    lockPoolsRead();
    for (auto& set : results) { cb(set); }
    unlockPoolsRead();
    viewLock.unlockRead();
}

template<typename... TComponents>
void View<TComponents...>::removeEntity(Entity entity) {
    lockWrite();
    containedEntities[entity] = false;
    for (auto it = results.begin(); it != results.end(); ++it) {
        if (it->entity() == entity) {
            results.erase(it);
            break;
        }
    }
    unlockWrite();
}

template<typename... TComponents>
void View<TComponents...>::tryAddEntity(Registry& reg, Entity ent) {
    lockWrite();
    if (containedEntities[ent]) {
        unlockWrite();
        return;
    }
    results.emplace_back(reg, ent);
    if (!results.back().isValid()) { results.pop_back(); }
    else {
        containedEntities[ent] = true;
    }
    unlockWrite();
}

template<typename... TComponents>
void View<TComponents...>::clearAndRefresh(Registry& reg) {
    lockWrite();
    results.clear();
    for (auto it = containedEntities.begin(); it != containedEntities.end(); ++it) { *it = false; }
    reg.populateView(*this);
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