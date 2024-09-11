#ifndef BLIB_ECS_TRANSACTION_HPP
#define BLIB_ECS_TRANSACTION_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/ReadWriteLock.hpp>
#include <mutex>

namespace bl
{
namespace ecs
{
template<typename T>
class ComponentPool;
class Registry;

/// Types and helpers for Transactions
namespace tx
{
/**
 * @brief Enum describing the type of lock to acquire for entities in the registry
 *
 * @ingroup ECS
 */
enum EntityContext { EntityUnlocked, EntityRead, EntityWrite };

/**
 * @brief Deduction helper to use to create transactions to read certain types of components
 *
 * @tparam ...TComponents The types of components that will be read
 * @ingroup ECS
 */
template<typename... TComponents>
struct ComponentRead {};

/**
 * @brief Deduction helper to use to create transactions to write certain types of components
 *
 * @tparam ...TComponents The types of components that will be write
 * @ingroup ECS
 */
template<typename... TComponents>
struct ComponentWrite {};

} // namespace tx

/// Internal implementation details for transactions
namespace txp
{
class TransactionEntityUnlocked {
protected:
    TransactionEntityUnlocked(const Registry&) {}
    void unlock() {}
};

class TransactionEntityRead {
protected:
    TransactionEntityRead(const Registry&);
    void unlock() { lock.unlock(); }

private:
    std::unique_lock<std::recursive_mutex> lock;
};

class TransactionEntityWrite {
protected:
    TransactionEntityWrite(const Registry&);
    void unlock() { lock.unlock(); }
    operator const TransactionEntityRead&() const {
        return *reinterpret_cast<const TransactionEntityRead*>(this);
    }

private:
    std::unique_lock<std::recursive_mutex> lock;
};

template<tx::EntityContext Ctx>
struct TxEntityResolve;

template<>
struct TxEntityResolve<tx::EntityUnlocked> {
    using T = TransactionEntityUnlocked;
};

template<>
struct TxEntityResolve<tx::EntityRead> {
    using T = TransactionEntityRead;
};

template<>
struct TxEntityResolve<tx::EntityWrite> {
    using T = TransactionEntityWrite;
};

template<typename T>
class TransactionComponentRead {
protected:
    TransactionComponentRead(const Registry& registry);
    TransactionComponentRead(util::ReadWriteLock& lock)
    : lock(lock) {}
    void unlock() { lock.unlock(); }

private:
    util::ReadWriteLock::ReadScopeGuard lock;

    friend class ::bl::ecs::ComponentPool<T>;
    friend class ::bl::ecs::Registry;
};

template<typename T>
class TransactionComponentWrite {
protected:
    TransactionComponentWrite(const Registry& registry);
    TransactionComponentWrite(util::ReadWriteLock& lock)
    : lock(lock) {}
    void unlock() { lock.unlock(); }

private:
    util::ReadWriteLock::WriteScopeGuard lock;

    friend class ::bl::ecs::ComponentPool<T>;
    friend class ::bl::ecs::Registry;
};

} // namespace txp

template<tx::EntityContext EntityCtx, typename TComponentRead = tx::ComponentRead<>,
         typename TComponentWrite = tx::ComponentWrite<>>
class Transaction {};

/**
 * @brief Transaction that provides an external lock across the ECS
 *
 * @tparam ...TReadComs Component types that will be read
 * @tparam ...TWriteComs Component types that will be written
 * @tparam EntityCtx The type of lock to acquire for entities
 */
template<tx::EntityContext EntityCtx, typename... TReadComs, typename... TWriteComs>
class Transaction<EntityCtx, tx::ComponentRead<TReadComs...>, tx::ComponentWrite<TWriteComs...>>
: public txp::TxEntityResolve<EntityCtx>::T
, public txp::TransactionComponentRead<TReadComs>...
, public txp::TransactionComponentWrite<TWriteComs>...
, private util::NonCopyable {
    using EntityBase = txp::TxEntityResolve<EntityCtx>::T;

public:
    /**
     * @brief Creates the transaction and acquires the appropriate locks
     *
     * @param registry The ECs registry the transaction applies to
     */
    Transaction(const Registry& registry);

    /**
     * @brief Unlocks the affected portions of the ECS, ending the transaction
     */
    void unlock();

    /**
     * @brief Returns whether the transaction is still in progress
     */
    bool isLocked() const;

private:
    bool unlocked;
};

template<tx::EntityContext EntityCtx, typename... TReadComs, typename... TWriteComs>
Transaction<EntityCtx, tx::ComponentRead<TReadComs...>,
            tx::ComponentWrite<TWriteComs...>>::Transaction(const Registry& registry)
: EntityBase(registry)
, txp::TransactionComponentRead<TReadComs>(registry)...
, txp::TransactionComponentWrite<TWriteComs>(registry)... {}

template<tx::EntityContext EntityCtx, typename... TReadComs, typename... TWriteComs>
void Transaction<EntityCtx, tx::ComponentRead<TReadComs...>,
                 tx::ComponentWrite<TWriteComs...>>::unlock() {
    if (!unlocked) {
        unlocked = true;
        EntityBase::unlock();
        (txp::TransactionComponentRead<TReadComs>::unlock(), ...);
        (txp::TransactionComponentWrite<TWriteComs>::unlock(), ...);
    }
}

template<tx::EntityContext EntityCtx, typename... TReadComs, typename... TWriteComs>
bool Transaction<EntityCtx, tx::ComponentRead<TReadComs...>,
                 tx::ComponentWrite<TWriteComs...>>::isLocked() const {
    return !unlocked;
}

} // namespace ecs
} // namespace bl

#endif
