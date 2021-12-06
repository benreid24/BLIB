#include <BLIB/Serialization/JSON.hpp>

namespace bl
{
namespace serial
{
namespace json
{
bool SerializableObjectBase::deserialize(const Group& val, void* obj) const {
    for (const auto& field : fields) {
        const auto f = val.getField(field.first);
        if (f.has_value()) {
            if (!field.second->deserialize(f.value(), obj)) { return false; }
        }
        else {
            return false;
        }
    }
    return true;
}

Group SerializableObjectBase::serialize(const void* obj) const {
    Group group;
    for (const auto& field : fields) { group.addField(field.first, field.second->serialize(obj)); }
    return group;
}

} // namespace json
} // namespace serial
} // namespace bl
