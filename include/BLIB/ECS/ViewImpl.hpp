#ifndef BLIB_ECS_VIEWIMPL_HPP
#define BLIB_ECS_VIEWIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

#include <BLIB/ECS/TagsImpl.hpp>

namespace bl
{
namespace ecs
{

/**
 * @brief A view into the ECS providing fast iteration over entities with the given set of
 *        components. View results stay up to date as the component and entity data change
 *
 * @tparam ...TReqComs List of components that must be on an entity for the view to capture it
 * @tparam ...TOptComs List of extra components to fetch that do not affect filtering
 * @tparam ...TExcComs List of components to exclude entities that contain them
 * @ingroup ECS
 */
template<typename... TReqComs, typename... TOptComs, typename... TExcComs>
class View<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>>
: public priv::ViewBase {
private:
    using ViewTags = Tags<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>>;
    static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

public:
    /// The ComponentSet type contained in this View
    using TRow = typename ViewTags::TComponentSet;

    /**
     * @brief Destroy the View object
     *
     */
    virtual ~View() = default;

    /**
     * @brief Iterates over all results of the view
     *
     * @tparam TCallback The callback type to invoke
     * @param cb Handler for each entity result in the view
     */
    template<typename TCallback>
    void forEach(const TCallback& cb) {
        static_assert(std::is_invocable<TCallback, typename ViewTags::TComponentSet&>::value,
                      "visitor must have signature void(ComponentSet<...>&)");

        ensureUpdated();

        viewLock.lockRead();
        for (auto& set : results) { cb(set); }
        viewLock.unlockRead();
    }

    /**
     * @brief Fetches the view result for the given entity
     *
     * @param entity The entity to fetch the component set for
     * @return A pointer to the component set, or nullptr if the entity is not contained
     */
    typename ViewTags::TComponentSet* get(Entity entity) {
        const auto ei = entity.getIndex();
        const auto i  = ei < entityToIndex.size() ? entityToIndex[ei] : InvalidIndex;
        return i != InvalidIndex ? &results[i] : nullptr;
    }

private:
    Registry& registry;
    util::ReadWriteLock viewLock;
    std::array<ComponentPoolBase*, ViewTags::NumComponents> pools;
    std::vector<typename ViewTags::TComponentSet> results;
    std::vector<std::size_t> entityToIndex;

    std::mutex queueLock;
    std::vector<Entity> toAdd;
    std::vector<Entity> toRemove;

    View(Registry& reg)
    : ViewBase(ViewTags::createMask(reg), typeid(View))
    , registry(reg)
    , pools({&reg.getPool<TReqComs>()..., &reg.getPool<TOptComs>()...}) {
        results.reserve(256);
        entityToIndex.resize(256, InvalidIndex);
        toAdd.reserve(128);
        toRemove.reserve(128);
        reg.populateViewWithLock(*this);
        ensureUpdated();
    }

    virtual void removeEntity(Entity entity) override {
        std::unique_lock lock(queueLock);
        toRemove.emplace_back(entity);
    }

    virtual void nullEntityComponent(Entity entity, void* com) override {
        const std::uint64_t entIndex = entity.getIndex();
        if (entIndex < entityToIndex.size()) { results[entityToIndex[entIndex]].refresh(registry); }
    }

    virtual void tryAddEntity(Entity entity) override {
        std::unique_lock lock(queueLock);
        toAdd.emplace_back(entity);
    }

    virtual void clearAndRefresh() override {
        lockWrite();
        results.clear();
        std::fill(entityToIndex.begin(), entityToIndex.end(), InvalidIndex);
        registry.populateView(*this);
        unlockWrite();
        ensureUpdated();
    }

    void ensureUpdated() {
        std::unique_lock lock(queueLock);
        lockWrite();

        for (Entity ent : toAdd) {
            const std::uint64_t entIndex = ent.getIndex();
            if (entIndex + 1 > entityToIndex.size()) {
                entityToIndex.resize(entIndex + 1, InvalidIndex);
            }

            if (entityToIndex[entIndex] != InvalidIndex) { continue; }
            if (!mask.passes(registry.entityMasks[entIndex])) { continue; }

            const std::size_t ni = results.size();
            results.emplace_back(registry, ent);
            if (!results.back().isValid()) { results.pop_back(); }
            else { entityToIndex[entIndex] = ni; }
        }
        toAdd.clear();

        for (Entity ent : toRemove) {
            const std::uint64_t entIndex = ent.getIndex();
            if (entIndex + 1 > entityToIndex.size()) { continue; }

            std::size_t& index = entityToIndex[entIndex];
            if (index == InvalidIndex) { continue; }
            if (index != results.size() - 1) {
                std::size_t& backIndex = entityToIndex[results.back().entity().getIndex()];
                results[index]         = results.back();
                backIndex              = index;
            }
            results.pop_back();
            index = InvalidIndex;
        }
        toRemove.clear();

        unlockWrite();
    }

    void lockPoolsRead() {
        for (ComponentPoolBase* pool : pools) { pool->poolLock.lockRead(); }
    }

    void unlockPoolsRead() {
        for (ComponentPoolBase* pool : pools) { pool->poolLock.unlockRead(); }
    }

    void lockWrite() {
        viewLock.lockWrite();
        lockPoolsRead();
    }

    void unlockWrite() {
        viewLock.unlockWrite();
        unlockPoolsRead();
    }

    friend class Registry;
};

} // namespace ecs
} // namespace bl

#endif
