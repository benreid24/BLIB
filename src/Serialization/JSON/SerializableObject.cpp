#include <BLIB/Serialization/JSON.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace serial
{
namespace json
{
SerializableObjectBase::SerializableObjectBase(StrictMode&&)
: strict(true) {}

SerializableObjectBase::SerializableObjectBase(RelaxedMode&&)
: strict(false) {}

bool SerializableObjectBase::deserialize(const Group& val, void* obj) const {
    for (const auto& field : fields) {
        const auto* f = val.getField(field.first);
        if (f != nullptr) {
            if (!field.second->deserialize(*f, obj)) { return false; }
        }
        else if (strict || !field.second->optional()) {
            return false;
        }
        else {
            field.second->makeDefault(obj);
        }
    }
    // never care about extra fields
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
