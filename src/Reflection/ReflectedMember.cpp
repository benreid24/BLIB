#include <BLIB/Reflection/ReflectedMember.hpp>

namespace bl
{
namespace refl
{
ReflectedMemberBase::ReflectedMemberBase(std::uint16_t id, std::string_view name,
                                         std::type_index type)
: id(id)
, name(name)
, type(type) {}

} // namespace refl
} // namespace bl
