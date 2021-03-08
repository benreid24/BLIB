#include <BLIB/Files/Binary/SerializableField.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

namespace bl
{
namespace file
{
namespace binary
{
SerializableFieldBase::SerializableFieldBase(SerializableObject& owner, std::uint16_t id) {
    owner.addField(this, id);
}

} // namespace binary
} // namespace file
} // namespace bl
