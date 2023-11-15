#ifndef BLIB_ECS_FLAGS_HPP
#define BLIB_ECS_FLAGS_HPP

#include <cstdint>

namespace bl
{
namespace ecs
{
/**
 * @brief Collection of flags that may be set on an entity when created
 *
 * @ingroup ECS
 */
enum struct Flags : std::uint64_t {
    /// The entity has no special flags
    None = 0,

    /// The entity is a dummy entity. Affects the trait SkipDummy
    Dummy = 0x1 << 0
};

} // namespace ecs
} // namespace bl

template<typename T>
inline std::uint64_t operator<<(bl::ecs::Flags flags, T shift) {
    return static_cast<std::uint64_t>(flags) << shift;
}

template<typename T>
inline std::uint64_t operator>>(bl::ecs::Flags flags, T shift) {
    return static_cast<std::uint64_t>(flags) >> shift;
}

inline std::uint64_t operator&(bl::ecs::Flags left, bl::ecs::Flags right) {
    return static_cast<std::uint64_t>(left) & static_cast<std::uint64_t>(right);
}

#endif
