#ifndef BLIB_REFLECTION_SPEC_HPP
#define BLIB_REFLECTION_SPEC_HPP

#include <BLIB/Reflection/AttributeList.hpp>

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
    MemberList members;
    AttributeList<TAttrs...> attributes;
};

/**
 * @brief Deduction helper to create a Spec object for a reflected type
 *
 * @tparam ObjectType The type of object this spec describes
 * @tparam MemberList The type of the member list for this object
 * @tparam ...TAttrs The types of attributes to store on this object
 * @param members The member list for this object
 * @param ...attrs The attributes to store on this object
 * @return The spec for the object
 * @ingroup Reflection
 */
template<typename ObjectType, typename MemberList, typename... TAttrs>
Spec<ObjectType, MemberList, TAttrs...> makeSpec(MemberList members, TAttrs&&... attrs) {
    return Spec<ObjectType, MemberList, TAttrs...>{
        members, AttributeList<TAttrs...>{std::forward<TAttrs>(attrs)...}};
}

} // namespace refl
} // namespace bl

#endif
