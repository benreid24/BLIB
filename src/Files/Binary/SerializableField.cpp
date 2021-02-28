#include <BLIB/Files/Binary/SerializableField.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

namespace bl
{
namespace bf
{
SerializableFieldBase::SerializableFieldBase(SerializableObject& owner) { owner.addField(this); }

} // namespace bf
} // namespace bl
