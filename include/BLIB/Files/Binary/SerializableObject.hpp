#ifndef BLIB_FILES_BINARY_SERIALIZABLEOBJECT_HPP
#define BLIB_FILES_BINARY_SERIALIZABLEOBJECT_HPP

#include <BLIB/Files/Binary/BinaryFile.hpp>

#include <vector>

namespace bl
{
namespace bf
{
class SerializableFieldBase;

class SerializableObject {
public:
    SerializableObject() = default;

    SerializableObject(const SerializableObject& copy);

    SerializableObject(SerializableObject&& copy);

    SerializableObject& operator=(const SerializableObject& copy);

    bool serialize(BinaryFile& output) const;

    bool deserialize(BinaryFile& input);

private:
    std::vector<SerializableFieldBase*> fields;

    void addField(SerializableFieldBase* field);

    friend class SerializableFieldBase;
};

} // namespace bf
} // namespace bl

#endif
