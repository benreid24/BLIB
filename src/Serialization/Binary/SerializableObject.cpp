#include <BLIB/Serialization/Binary/SerializableField.hpp>
#include <BLIB/Serialization/Binary/SerializableObject.hpp>

namespace bl
{
namespace serial
{
namespace binary
{
bool SerializableObjectBase::serialize(OutputStream& stream, const void* obj) const {
    if (!stream.write<std::uint16_t>(fields.size())) return false;
    for (const auto& p : fields) {
        if (!stream.write<std::uint16_t>(p.first)) return false;
        if (!stream.write<std::uint32_t>(p.second->size(obj))) return false;
        if (!p.second->serialize(stream, obj)) return false;
    }
    return true;
}

bool SerializableObjectBase::deserialize(InputStream& stream, void* obj) const {
    std::uint16_t n = 0;
    if (!stream.read<std::uint16_t>(n)) return false;
    for (std::uint16_t i = 0; i < n; ++i) {
        std::uint16_t id    = 0;
        std::uint32_t fsize = 0;
        if (!stream.read<std::uint16_t>(id)) return false;
        if (!stream.read<std::uint32_t>(fsize)) return false;
        const auto it = fields.find(id);
        if (it != fields.end()) {
            if (!it->second->deserialize(stream, obj)) return false;
        }
        else {
            if (!stream.skip(fsize)) return false;
        }
    }
    return true;
}

std::size_t SerializableObjectBase::size(const void* obj) const {
    std::size_t s = sizeof(std::uint16_t);
    for (const auto f : fields) {
        s += sizeof(std::uint16_t) + sizeof(std::uint32_t);
        s += f.second->size(obj);
    }
    return s;
}

} // namespace binary
} // namespace serial
} // namespace bl
