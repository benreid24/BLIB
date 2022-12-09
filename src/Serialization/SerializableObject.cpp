#include <BLIB/Serialization/SerializableField.hpp>
#include <BLIB/Serialization/SerializableObject.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Serialization/JSON/JSONLoader.hpp>
#include <BLIB/Util/StreamUtil.hpp>

namespace bl
{
namespace serial
{
namespace
{
constexpr unsigned int MaxFieldCount = 128;
}

SerializableObjectBase::SerializableObjectBase(const std::string& name)
: debugName(name) {}

bool SerializableObjectBase::deserializeJSON(const json::Group& val, void* obj) const {
    for (const auto& field : fieldsJson) {
        const auto* f = val.getField(field.first);
        if (f != nullptr) {
            if (!field.second->deserializeJSON(*f, obj)) {
                BL_LOG_DEBUG << "Failed to deserialize '" << debugName << "', field failed: '"
                             << field.first << "'";
                return false;
            }
        }
        else if (!field.second->optional()) {
            BL_LOG_DEBUG << "Failed to deserialize '" << debugName << "', required field '"
                         << field.first << "' missing";
            return false;
        }
        else { field.second->makeDefault(obj); }
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
    char foundFields[MaxFieldCount];
    std::memset(foundFields, 0, MaxFieldCount);

    const auto logReadError = [this]() {
        BL_LOG_DEBUG << "Failed to deserialize '" << debugName << "', bad read";
    };

    std::uint16_t n = 0;
    if (!stream.read<std::uint16_t>(n)) {
        logReadError();
        return false;
    }
    for (std::uint16_t i = 0; i < n; ++i) {
        std::uint16_t id    = 0;
        std::uint32_t fsize = 0;
        if (!stream.read<std::uint16_t>(id)) {
            logReadError();
            return false;
        }
        if (!stream.read<std::uint32_t>(fsize)) {
            logReadError();
            return false;
        }
        const auto it = fieldsBinary.find(id);
        if (it != fieldsBinary.end()) {
            if (id >= MaxFieldCount) {
                BL_LOG_CRITICAL << "Field id cannot be greater than " << MaxFieldCount
                                << ". Message type: " << debugName;
                return false;
            }
            if (!it->second->deserializeBinary(stream, obj)) {
                BL_LOG_DEBUG << "Failed to deserialize '" << debugName << "' field failed: " << id;
                return false;
            }
            foundFields[id] = 1;
        }
        else {
            if (!stream.skip(fsize)) {
                logReadError();
                return false;
            }
        }
    }

    // default fields not found and fail if any are required
    for (const auto& field : fieldsBinary) {
        if (foundFields[field.first] != 0) continue;
        if (!field.second->optional()) {
            BL_LOG_DEBUG << "Failed to deserialize '" << debugName << "', missing required field "
                         << field.first;
            return false;
        }
        field.second->makeDefault(obj);
    }
    return true;
}

std::size_t SerializableObjectBase::binarySize(const void* obj) const {
    std::size_t s = sizeof(std::uint16_t);
    for (const auto& f : fieldsBinary) {
        s += sizeof(std::uint16_t) + sizeof(std::uint32_t);
        s += f.second->binarySize(obj);
    }
    return s;
}

bool SerializableObjectBase::serializePackedBinary(binary::OutputStream& stream,
                                                   const void* obj) const {
    for (const auto& field : sortedFields) {
        if (!field.second->serializeBinary(stream, obj)) return false;
    }
    return true;
}

bool SerializableObjectBase::deserializePackedBinary(binary::InputStream& stream, void* obj) const {
    for (const auto& field : sortedFields) {
        if (!field.second->deserializeBinary(stream, obj)) return false;
    }
    return true;
}

bool SerializableObjectBase::serializeJsonStream(std::ostream& stream, const void* obj,
                                                 unsigned int ts, unsigned int ilvl) {
    stream << "{";
    if (ts > 0) stream << '\n';

    const unsigned int sc = ilvl + ts;
    unsigned int i        = 0;
    for (const auto& field : fieldsJson) {
        util::StreamUtil::writeRepeated(stream, ' ', sc);
        stream << '"' << field.first << "\": ";
        field.second->serializeJsonStream(stream, obj, ts, sc);
        if (i < fieldsJson.size() - 1) stream << ",";
        ++i;
        if (ts > 0) stream << '\n';
    }

    util::StreamUtil::writeRepeated(stream, ' ', ilvl);
    stream << "}";

    return stream.good();
}

bool SerializableObjectBase::deserializeJsonStream(std::istream& stream, void* obj) {
    using FIter = decltype(fieldsJson)::iterator;
    std::pair<bool, FIter> fields[MaxFieldCount];
    unsigned int fieldCount = 0;
    for (auto it = fieldsJson.begin(); it != fieldsJson.end(); ++it) {
        fields[fieldCount].first  = false;
        fields[fieldCount].second = it;
        ++fieldCount;
    }

    const auto markVisited = [&fields, fieldCount](FIter it) {
        for (unsigned int i = 0; i < fieldCount; ++i) {
            if (fields[i].second == it) {
                fields[i].first = true;
                break;
            }
        }
    };

    const auto logParseError = [this](const std::string& err) {
        BL_LOG_DEBUG << "Failed to deserialize '" << debugName << "', parse error: " << err;
    };

    util::StreamUtil::skipWhitespace(stream);
    if (stream.peek() != '{') {
        logParseError(std::string("Expected '{' but got ") + static_cast<char>(stream.peek()));
        return false;
    }
    stream.get();

    while (stream.good()) {
        util::StreamUtil::skipWhitespace(stream);
        if (stream.peek() != '"') {
            logParseError(std::string("Expected '\"' but got ") + static_cast<char>(stream.peek()));
            return false;
        }
        stream.get();

        std::string name;
        std::getline(stream, name, '"');
        if (!stream.good()) {
            logParseError("End of stream while reading field name");
            return false;
        }
        if (!util::StreamUtil::skipUntil(stream, ':')) {
            logParseError("End of stream while reading to ':'");
            return false;
        }
        stream.get();

        const auto it = fieldsJson.find(name);
        if (it != fieldsJson.end()) {
            if (!it->second->deserializeJsonStream(stream, obj)) {
                BL_LOG_DEBUG << "Failed to deserialize '" << debugName << "', field failed '"
                             << it->first << "'";
                return false;
            }
            markVisited(it);
        }
        else {
            json::Loader loader(stream);
            json::Value trash(false);
            if (!loader.loadValue(trash)) {
                logParseError("Failed to skip past unknown json data");
                return false;
            }
        }

        util::StreamUtil::skipWhitespace(stream);
        const char c = stream.get();
        if (c == '}') {
            // check fields not found for required
            for (unsigned int i = 0; i < fieldCount; ++i) {
                if (!fields[i].first) {
                    const auto& field = fields[i].second;
                    if (!field->second->optional()) {
                        BL_LOG_DEBUG << "Failed to deserialize '" << debugName
                                     << "', missing required field '" << field->first << "'";
                        return false;
                    }
                    field->second->makeDefault(obj);
                }
            }
            return true;
        }
        else if (c == ',') { continue; }
        logParseError(std::string("Expected ',' but got ") + c);
        return false;
    }
    logParseError("Unexpected end of stream");
    return false;
}

} // namespace serial
} // namespace bl
