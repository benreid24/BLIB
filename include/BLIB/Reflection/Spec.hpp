#ifndef BLIB_REFLECTION_SPEC_HPP
#define BLIB_REFLECTION_SPEC_HPP

#include <BLIB/Reflection/AttributeList.hpp>
#include <string_view>
#include <tuple>

namespace bl
{
namespace refl
{
/**
 * @brief Reflection spec for objects. Contains a member list and a set of arbitrary attributes
 *
 * @tparam ObjectType The type of object this spec describes
 * @tparam MemberList The type of the member list for this object
 * @tparam ...TAttrs The types of attributes to store on this object
 * @ingroup Reflection
 */
template<typename ObjectType, typename MemberList, typename... TAttrs>
struct Spec {
    std::string_view name;
    MemberList members;
    AttributeList<TAttrs...> attributes;
    constexpr static std::size_t memberCount = std::tuple_size<MemberList>::value;
};

/**
 * @brief Deduction helper to create a Spec object for a reflected type
 *
 * @tparam ObjectType The type of object this spec describes
 * @tparam MemberList The type of the member list for this object
 * @tparam ...TAttrs The types of attributes to store on this object
 * @param name The name of the object type
 * @param members The member list for this object
 * @param ...attrs The attributes to store on this object
 * @return The spec for the object
 * @ingroup Reflection
 */
template<typename ObjectType, typename MemberList, typename... TAttrs>
Spec<ObjectType, MemberList, TAttrs...> makeSpec(std::string_view name, MemberList members,
                                                 TAttrs&&... attrs) {
    return Spec<ObjectType, MemberList, TAttrs...>{
        name, members, AttributeList<TAttrs...>{std::forward<TAttrs>(attrs)...}};
}

} // namespace refl
} // namespace bl

#endif
