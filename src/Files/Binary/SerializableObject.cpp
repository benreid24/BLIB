#include <BLIB/Files/Binary/SerializableField.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace file
{
namespace binary
{
SerializableObject::SerializableObject(const SerializableObject&) {}

SerializableObject::SerializableObject(SerializableObject&&) {}

SerializableObject& SerializableObject::operator=(const SerializableObject&) { return *this; }

bool SerializableObject::serialize(File& output) const {
    if (!output.write<std::uint16_t>(fields.size())) return false;
    for (const auto& it : fields) {
        if (!output.write<std::uint16_t>(it.first)) return false;
        if (!output.write<std::uint32_t>(it.second->size())) return false;
        if (!it.second->serialize(output)) return false;
    }
    return true;
}

bool SerializableObject::deserialize(File& input) {
    std::uint16_t nfields = 0;
    if (!input.read<std::uint16_t>(nfields)) return false;
    for (std::uint16_t i = 0; i < nfields; ++i) {
        std::uint16_t fid = 0;
        std::uint32_t fs  = 0;
        if (!input.read<std::uint16_t>(fid)) return false;
        if (!input.read<std::uint32_t>(fs)) return false;
        auto it = fields.find(fid);
        if (it != fields.end()) {
            if (!it->second->deserialize(input)) return false;
        }
        else {
            if (!input.skip(fs)) return false;
        }
    }
    return true;
}

void SerializableObject::addField(SerializableFieldBase* field, std::uint16_t id) {
    if (fields.find(id) != fields.end()) {
        BL_LOG_ERROR << "SerializableObject has duplicate field id: " << id;
        return;
    }
    fields[id] = field;
}

std::uint32_t SerializableObject::size() const {
    std::uint32_t s(0);
    for (const auto& it : fields) { s += it.second->size(); }
    return s;
}

} // namespace binary
} // namespace file
} // namespace bl
