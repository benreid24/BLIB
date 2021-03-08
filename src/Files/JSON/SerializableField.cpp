#include <BLIB/Files/JSON/SerializableField.hpp>

namespace bl
{
namespace file
{
namespace json
{
SerializableFieldBase::SerializableFieldBase(const std::string& name, SerializableObject& owner)
: name(name) {
    owner.addField(this);
}

const std::string& SerializableFieldBase::getName() const { return name; }

} // namespace json
} // namespace file
} // namespace bl
