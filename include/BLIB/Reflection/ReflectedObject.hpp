#ifndef BLIB_REFLECTION_OBJECT_HPP
#define BLIB_REFLECTION_OBJECT_HPP

#include <BLIB/Reflection/AttributeList.hpp>
#include <BLIB/Reflection/Detail/IsMember.hpp>
#include <BLIB/Reflection/Detail/IsSpec.hpp>
#include <BLIB/Reflection/ReflectedMember.hpp>
#include <BLIB/Reflection/Spec.hpp>

namespace bl
{
namespace refl
{
/**
 * @brief The core piece of the reflection system. Specialize this class for any type that requires
 *        reflection. The spec() member must return the result of a call to makeSpec
 *
 * @tparam ObjectType The type to reflect
 * @ingroup Reflection
 */
template<typename ObjectType>
struct ReflectedObject {
    static_assert(false, "No reflection spec defined for this object type");

    inline static const auto spec = makeSpec<void>("", memberList());
};

/**
 * @brief Visit all members of a reflected object
 *
 * @tparam VisitorType The type of the visitor function or class
 * @tparam ObjectType The type of object to visit
 * @param object The object to visit
 * @param visitor The visitor to call
 * @ingroup Reflection
 */
template<typename VisitorType, typename ObjectType>
void visit(ObjectType& object, VisitorType&& visitor) {
    using Reflected = ReflectedObject<ObjectType>;

    using TSpec = std::decay_t<decltype(ReflectedObject<ObjectType>::spec)>;
    static_assert(detail::IsSpec<TSpec>::value,
                  "ReflectedObject<T>::spec() must return a refl::Spec");

    std::apply(
        [&object, &visitor](const auto&... member) {
            static_assert((detail::IsMemberT<std::decay_t<decltype(member)>>::value && ...),
                          "ReflectedObject members must be of type Member");
            (visitor(member, member.getMember(object)), ...);
        },
        Reflected::spec.members);
}

/**
 * @brief Visit all members of a reflected object
 *
 * @tparam VisitorType The type of the visitor function or class
 * @tparam ObjectType The type of object to visit
 * @param object The object to visit
 * @param visitor The visitor to call
 * @ingroup Reflection
 */
template<typename VisitorType, typename ObjectType>
void visit(const ObjectType& object, VisitorType&& visitor) {
    using Reflected = ReflectedObject<ObjectType>;

    using TSpec = std::decay_t<decltype(ReflectedObject<ObjectType>::spec)>;
    static_assert(detail::IsSpec<TSpec>::value,
                  "ReflectedObject<T>::spec() must return a refl::Spec");

    std::apply(
        [&object, &visitor](const auto&... member) {
            static_assert((detail::IsMemberT<std::decay_t<decltype(member)>>::value && ...),
                          "ReflectedObject members must be of type Member");
            (visitor(member, member.getMember(object)), ...);
        },
        Reflected::spec.members);
}

/**
 * @brief Convenience accessor for the name of a reflected type
 *
 * @tparam T The object type to get the name of
 * @ingroup Reflection
 */
template<typename T>
constexpr std::string_view name = ReflectedObject<T>::spec.name;

/**
 * @brief Convenience accessor for the number of members in a reflected type
 *
 * @tparam ObjectType The object type to get the member count of
 * @ingroup Reflection
 */
template<typename ObjectType>
constexpr std::size_t memberCount = ReflectedObject<ObjectType>::spec.memberCount;

} // namespace refl
} // namespace bl

#endif
