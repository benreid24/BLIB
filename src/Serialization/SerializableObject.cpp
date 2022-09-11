#include <BLIB/Serialization/SerializableField.hpp>
#include <BLIB/Serialization/SerializableObject.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace serial
{
bool SerializableObjectBase::deserializeJSON(const json::Group& val, void* obj) const {
    for (const auto& field : fieldsJson) {
        const auto* f = val.getField(field.first);
        if (f != nullptr) {
            if (!field.second->deserializeJSON(*f, obj)) { return false; }
        }
        else if (!field.second->optional()) {
            return false;
        }
        else {
            field.second->makeDefault(obj);
        }
    }
    // never care about extra fields
    return true;
}

json::Group SerializableObjectBase::serializeJSON(const void* obj) const {
    json::Group group;
    for (const auto& field : fieldsJson) {
        group.addField(field.first, field.second->serializeJSON(obj));
    }
    return group;
}

bool SerializableObjectBase::serializeBinary(binary::OutputStream& stream, const void* obj) const {
    if (!stream.write<std::uint16_t>(fieldsBinary.size())) return false;
    for (const auto& p : fieldsBinary) {
        if (!stream.write<std::uint16_t>(p.first)) return false;
        if (!stream.write<std::uint32_t>(p.second->binarySize(obj))) return false;
        if (!p.second->serializeBinary(stream, obj)) return false;
    }
    return true;
}

bool SerializableObjectBase::deserializeBinary(binary::InputStream& stream, void* obj) const {
    std::unordered_map<std::uint16_t, const SerializableFieldBase*> fields = fieldsBinary;

    std::uint16_t n = 0;
    if (!stream.read<std::uint16_t>(n)) return false;
    for (std::uint16_t i = 0; i < n; ++i) {
        std::uint16_t id    = 0;
        std::uint32_t fsize = 0;
        if (!stream.read<std::uint16_t>(id)) return false;
        if (!stream.read<std::uint32_t>(fsize)) return false;
        const auto it = fields.find(id);
        if (it != fields.end()) {
            if (!it->second->deserializeBinary(stream, obj)) return false;
            fields.erase(it);
        }
        else {
            if (!stream.skip(fsize)) return false;
        }
    }

    // default fields not found and fail if any are required
    for (const auto& field : fields) {
        if (!field.second->optional()) { return false; }
        field.second->makeDefault(obj);
    }
    return true;
}

std::size_t SerializableObjectBase::binarySize(const void* obj) const {
    std::size_t s = sizeof(std::uint16_t);
    for (const auto f : fieldsBinary) {
        s += sizeof(std::uint16_t) + sizeof(std::uint32_t);
        s += f.second->binarySize(obj);
    }
    return s;
}

} // namespace serial
} // namespace bl
