#include <BLIB/Files/JSON/SerializableField.hpp>
#include <BLIB/Files/JSON/SerializableObject.hpp>

namespace bl
{
namespace file
{
namespace json
{
bool SerializableObject::deserialize(const Group& group) {
    for (SerializableFieldBase* field : fields) {
        if (!group.hasField(field->getName())) return false;
        if (!field->deserialize(group.getField(field->getName()).value())) return false;
    }
    return true;
}

Group SerializableObject::serialize() const {
    Group group;

    for (SerializableFieldBase* field : fields) {
        group.addField(field->getName(), field->serialize());
    }

    return group;
}

void SerializableObject::addField(SerializableFieldBase* field) { fields.push_back(field); }

SerializableObject& SerializableObject::operator=(const SerializableObject&) { return *this; }

SerializableObject::SerializableObject(const SerializableObject&) {}

SerializableObject::SerializableObject(SerializableObject&&) {}

} // namespace json
} // namespace file
} // namespace bl
