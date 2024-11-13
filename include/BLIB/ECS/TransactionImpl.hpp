#ifndef BLIB_ECS_TRANSACTIONIMPL_HPP
#define BLIB_ECS_TRANSACTIONIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

#include <BLIB/ECS/Transaction.hpp>

namespace bl
{
namespace ecs
{
namespace txp
{
inline TransactionEntityWrite::TransactionEntityWrite(Registry& r)
: lock(r.entityLock) {}

inline TransactionEntityRead::TransactionEntityRead(Registry& r)
: lock(r.entityLock) {}

template<typename T>
TransactionComponentRead<T>::TransactionComponentRead(Registry& registry)
: lock(registry.getPool<T>().poolLock) {}

template<typename T>
TransactionComponentWrite<T>::TransactionComponentWrite(Registry& registry)
: lock(registry.getPool<T>().poolLock) {}

} // namespace txp

} // namespace ecs
} // namespace bl

#endif