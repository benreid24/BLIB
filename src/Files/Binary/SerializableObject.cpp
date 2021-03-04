#include <BLIB/Files/Binary/SerializableField.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

namespace bl
{
namespace bf
{
SerializableObject::SerializableObject(const SerializableObject&) {}

SerializableObject::SerializableObject(SerializableObject&&) {}

SerializableObject& SerializableObject::operator=(const SerializableObject&) { return *this; }

bool SerializableObject::serialize(BinaryFile& output) const {
    for (SerializableFieldBase* field : fields) {
        if (!field->serialize(output)) return false;
    }
    return true;
}

bool SerializableObject::deserialize(BinaryFile& input) {
    for (SerializableFieldBase* field : fields) {
        if (!field->deserialize(input)) return false;
    }
    return true;
}

void SerializableObject::addField(SerializableFieldBase* field) { fields.push_back(field); }

} // namespace bf
} // namespace bl
