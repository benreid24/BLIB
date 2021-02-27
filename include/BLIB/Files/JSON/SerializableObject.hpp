#ifndef BLIB_FILES_JSON_SERIALIZABLEOBJECT_HPP
#define BLIB_FILES_JSON_SERIALIZABLEOBJECT_HPP

#include <BLIB/Files/JSON/JSON.hpp>
#include <vector>

namespace bl
{
namespace json
{
class SerializableFieldBase;

class SerializableObject {
public:
    bool deserialize(const Group& value);

    Group serialize() const;

private:
    std::vector<SerializableFieldBase*> fields;

    void addField(SerializableFieldBase* field);

    friend class SerializableFieldBase;
};

} // namespace json
} // namespace bl

#endif
