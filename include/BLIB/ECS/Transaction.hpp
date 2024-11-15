#ifndef BLIB_ECS_TRANSACTION_HPP
#define BLIB_ECS_TRANSACTION_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/ReadWriteLock.hpp>
#include <BLIB/Util/VariadicHelpers.hpp>
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
enum EntityContext { EntityUnlocked = 0, EntityRead = 1, EntityWrite = 2 };

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
template<typename T>
class TransactionComponentWrite;
class TransactionEntityRead;
class TransactionEntityWrite;

class TransactionEntityUnlocked {
protected:
    TransactionEntityUnlocked() {}
    TransactionEntityUnlocked(const TransactionEntityRead&) {}
    TransactionEntityUnlocked(const TransactionEntityWrite&) {}
    TransactionEntityUnlocked(const TransactionEntityUnlocked&) {}
    TransactionEntityUnlocked(Registry&) {}
    void unlock() {}
};

class TransactionEntityRead {
protected:
    TransactionEntityRead(const TransactionEntityWrite&) {}
    TransactionEntityRead(const TransactionEntityRead&) {}
    TransactionEntityRead(Registry&);
    void unlock() {
        if (lock.has_value()) { lock.value().unlock(); }
    }

private:
    std::optional<std::unique_lock<std::recursive_mutex>> lock;
};

class TransactionEntityWrite {
protected:
    TransactionEntityWrite(const TransactionEntityWrite&) {}
    TransactionEntityWrite(Registry&);
    void unlock() {
        if (lock.has_value()) { lock.value().unlock(); }
    }

private:
    std::optional<std::unique_lock<std::recursive_mutex>> lock;
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
    TransactionComponentRead(const TransactionComponentWrite<T>&) {}
    TransactionComponentRead(const TransactionComponentRead&) {}
    TransactionComponentRead(Registry& registry);
    TransactionComponentRead(util::ReadWriteLock& lock)
    : lock(lock) {}
    void unlock() {
        if (lock.has_value()) { lock.value().unlock(); }
    }

private:
    std::optional<util::ReadWriteLock::ReadScopeGuard> lock;

    friend class ::bl::ecs::ComponentPool<T>;
    friend class ::bl::ecs::Registry;
};

template<typename T>
class TransactionComponentWrite {
protected:
    TransactionComponentWrite(const TransactionComponentWrite&) {}
    TransactionComponentWrite(Registry& registry);
    TransactionComponentWrite(util::ReadWriteLock& lock)
    : lock(lock) {}
    void unlock() {
        if (lock.has_value()) { lock.value().unlock(); }
    }

private:
    std::optional<util::ReadWriteLock::WriteScopeGuard> lock;

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
    using EntityBase = typename txp::TxEntityResolve<EntityCtx>::T;

public:
    /**
     * @brief Creates the transaction and acquires the appropriate locks
     *
     * @param registry The ECs registry the transaction applies to
     */
    Transaction(Registry& registry);

    /**
     * @brief Helper function to pass existing transactions to methods expecting different, but
     *        compatible, transactions
     *
     * @tparam ...TOtherReads The source transaction component read types
     * @tparam ...TOtherWrites The source transaction component write types
     * @tparam OtherEntityCtx The source transaction entity context
     * @param tx The source transaction
     */
    template<tx::EntityContext OtherEntityCtx, typename... TOtherReads, typename... TOtherWrites>
    Transaction(const Transaction<OtherEntityCtx, tx::ComponentRead<TOtherReads...>,
                                  tx::ComponentWrite<TOtherWrites...>>& tx);

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

    Transaction(util::ReadWriteLock& lock);

    template<typename T>
    friend class ::bl::ecs::ComponentPool;
};

template<tx::EntityContext EntityCtx, typename... TReadComs, typename... TWriteComs>
Transaction<EntityCtx, tx::ComponentRead<TReadComs...>,
            tx::ComponentWrite<TWriteComs...>>::Transaction(Registry& registry)
: EntityBase(registry)
, txp::TransactionComponentRead<TReadComs>(registry)...
, txp::TransactionComponentWrite<TWriteComs>(registry)... {}

template<tx::EntityContext EntityCtx, typename... TReadComs, typename... TWriteComs>
Transaction<EntityCtx, tx::ComponentRead<TReadComs...>,
            tx::ComponentWrite<TWriteComs...>>::Transaction(util::ReadWriteLock& lock)
: EntityBase()
, txp::TransactionComponentRead<TReadComs>(lock)...
, txp::TransactionComponentWrite<TWriteComs>(lock)... {
    static_assert(EntityCtx == tx::EntityUnlocked,
                  "Transaction constructed from component lock but requires entity lock too");
}

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

template<tx::EntityContext EntityCtx, typename... TReadComs, typename... TWriteComs>
template<tx::EntityContext OtherEntityCtx, typename... TOtherReads, typename... TOtherWrites>
Transaction<EntityCtx, tx::ComponentRead<TReadComs...>, tx::ComponentWrite<TWriteComs...>>::
    Transaction(const Transaction<OtherEntityCtx, tx::ComponentRead<TOtherReads...>,
                                  tx::ComponentWrite<TOtherWrites...>>& tx)
: EntityBase(tx)
, txp::TransactionComponentRead<TReadComs>(tx)...
, txp::TransactionComponentWrite<TWriteComs>(tx)... {
    static_assert(EntityCtx <= OtherEntityCtx,
                  "Source transaction must have same or greater entity ctx");

    static_assert(
        util::VariadicSetsContained<util::Variadic<TReadComs...>,
                                    util::Variadic<TOtherReads..., TOtherWrites...>>::value,
        "Read components must be contained by source transaction read + write sets");

    static_assert(util::VariadicSetsContained<util::Variadic<TWriteComs...>,
                                              util::Variadic<TOtherWrites...>>::value,
                  "Write components must be contained by source transaction write set");
}

} // namespace ecs
} // namespace bl

#endif
