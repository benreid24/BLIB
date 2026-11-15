#ifndef BLIB_SERIALIZATION_JSON_SERIALIZER_HPP
#define BLIB_SERIALIZATION_JSON_SERIALIZER_HPP

#include <BLIB/Reflection/ProvidedTypes.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Serialization/JSON/JSONLoader.hpp>
#include <BLIB/Streams/InputStream.hpp>
#include <BLIB/Streams/OutputStream.hpp>
#include <BLIB/Util/Base64.hpp>

#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Scripts/Value.hpp>
#include <BLIB/Serialization/SerializationError.hpp>
#include <BLIB/Streams/StreamUtil.hpp>
#include <BLIB/Util/UnderlyingType.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace bl
{
namespace serial
{
namespace json
{
using ::bl::stream::StreamUtil;

/**
 * @brief Helper class for SerializableField. Specializations are provided for json native types,
 *        vector, and unordered_map. User defined types should either implement ReflectedObject
 *        or provide a specialization of this class
 *
 * @tparam T The type to serialize
 * @tparam Helper for specializations, ignore
 * @ingroup JSON
 */
template<typename T,
         bool = (std::is_integral_v<T> || std::is_enum_v<T> || std::is_floating_point_v<T>) &&
                !std::is_same_v<T, bool>>
struct Serializer {
    /**
     * @brief Deserializes the given object from the json tree
     *
     * @param result The object to read into
     * @param value The JSON tree to read from
     */
    static void deserialize(T& result, const Value& value);

    /**
     * @brief Deserialize the given object from the given Value, which must be a group
     *
     * @param value The value to deserialize from
     * @param name The name of the field to read
     * @param result The object to read into
     */
    static void deserializeFrom(const Value& value, const std::string& name, T& result);

    /**
     * @brief Reads the value directly from the given stream instead of decoding into JSON objects
     *        first. More memory and time efficient
     *
     * @param stream The stream to read from
     * @param result The value to read into
     */
    static void deserializeStream(stream::InputStream& stream, T& result);

    /**
     * @brief Serializes the given value into a JSON Value
     *
     * @param value The value to serialize
     * @return Value A JSON object representing the value
     */
    static Value serialize(const T& value);

    /**
     * @brief Serialize the given object into the given value
     *
     * @param result The value to serialize into. Must be a Group
     * @param name The name of the field to write to
     * @param value The value to write
     * @return True on success, false on error
     */
    static void serializeInto(Group& result, const std::string& name, const T& value);

    /**
     * @brief Serializes the given value directly to the given stream instead of creating
     *        intermediate JSON objects first
     *
     * @param stream The stream to write to
     * @param value The value to write
     * @param tabSize Spaces to indent per level
     * @param currentIndent Current level of indentation in spaces
     */
    static void serializeStream(stream::OutputStream& stream, const T& value, unsigned int tabSize,
                                unsigned int currentIndent = 0);
};

/**
 * @brief Checked interface for Serializer that catches exceptions and returns status instead
 *
 * @tparam T The type to serialize or deserialize
 * @ingroup JSON
 */
template<typename T>
struct SerializerChecked {
    /**
     * @brief Deserializes the given object from the json tree
     *
     * @param result The object to read info
     * @param value The JSON tree to read from
     * @return True on success, false on error
     */
    static bool deserialize(const Value& value, T& result) {
        try {
            Serializer<T>::deserialize(result, value);
            return true;
        } catch (const SerializationError& e) {
            BL_LOG_ERROR << "Failed to deserialize type " << refl::getNameSafe<T>() << ": "
                         << e.what();
            return false;
        }
    }

    /**
     * @brief Deserialize the given object from the given Value, which must be a group
     *
     * @param value The value to deserialize from
     * @param name The name of the field to read
     * @param result The object to read into
     * @return True on success, false on error
     */
    static bool deserializeFrom(const Value& value, const std::string& name, T& result) {
        try {
            Serializer<T>::deserializeFrom(value, name, result);
            return true;
        } catch (const SerializationError& e) {
            BL_LOG_ERROR << "Failed to deserialize type " << refl::getNameSafe<T>() << ": "
                         << e.what();
            return false;
        }
    }

    /**
     * @brief Reads the value directly from the given stream instead of decoding into JSON objects
     *        first. More memory and time efficient
     *
     * @param stream The stream to read from
     * @param result The value to read into
     * @return True on success, false on error
     */
    static bool deserializeStream(stream::InputStream& stream, T& result) {
        try {
            Serializer<T>::deserializeStream(stream, result);
            return true;
        } catch (const SerializationError& e) {
            BL_LOG_ERROR << "Failed to deserialize type " << refl::getNameSafe<T>() << ": "
                         << e.what();
            return false;
        }
    }

    /**
     * @brief Serializes the given value into a JSON Value
     *
     * @param value The value to serialize
     * @return Value A JSON object representing the value
     */
    static Value serialize(const T& value) {
        try {
            return Serializer<T>::serialize(value);
        } catch (const SerializationError& e) {
            BL_LOG_ERROR << "Failed to serialize type " << refl::getNameSafe<T>() << ": "
                         << e.what();
            return Value();
        }
    }

    /**
     * @brief Serialize the given object into the given value
     *
     * @param result The value to serialize into. Must be a Group
     * @param name The name of the field to write to
     * @param value The value to write
     * @return True on success, false on error
     */
    static bool serializeInto(Group& result, const std::string& name, const T& value) {
        try {
            Serializer<T>::serializeInto(result, name, value);
            return true;
        } catch (const SerializationError& e) {
            BL_LOG_ERROR << "Failed to serialize type " << refl::getNameSafe<T>() << ": "
                         << e.what();
            return false;
        }
    }

    /**
     * @brief Serializes the given value directly to the given stream instead of creating
     *        intermediate JSON objects first
     *
     * @param stream The stream to write to
     * @param value The value to write
     * @param tabSize Spaces to indent per level
     * @param currentIndent Current level of indentation in spaces
     * @return True on success, false on error
     */
    static bool serializeStream(stream::OutputStream& stream, const T& value, unsigned int tabSize,
                                unsigned int currentIndent = 0) {
        try {
            Serializer<T>::serializeStream(stream, value, tabSize, currentIndent);
            return true;
        } catch (const SerializationError& e) {
            BL_LOG_ERROR << "Failed to serialize type " << refl::getNameSafe<T>() << ": "
                         << e.what();
            return false;
        }
    }
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

/// Private inline helper methods
namespace priv
{
template<typename T>
struct Serializer {
    template<typename F>
    static void deserializeFrom(const Value& val, const std::string& name, T& result,
                                F deserialize) {
        const auto* og = val.getAsGroup();
        if (og == nullptr) { throw SerializationError("JSON value was not object type: " + name); }
        const auto* f = og->getField(name);
        if (f == nullptr) { throw SerializationError("JSON field not found: " + name); }
        deserialize(result, *f);
    }

    template<typename F>
    static void serializeInto(Group& group, const std::string& name, const T& val, F serialize) {
        if (group.hasField(name)) {
            BL_LOG_WARN << "Type " << typeid(T).name() << " has duplicate field " << name;
        }
        group.addField(name, serialize(val));
    }
};

} // namespace priv

#include <BLIB/Serialization/JSON/Detail/DeserializeStreamVisitor.inl>
#include <BLIB/Serialization/JSON/Detail/DeserializeVisitor.inl>
#include <BLIB/Serialization/JSON/Detail/SerializeStreamVisitor.inl>
#include <BLIB/Serialization/JSON/Detail/SerializeVisitor.inl>

template<typename T>
struct Serializer<T, false> {
    static void deserialize(T& result, const Value& value) {
        try {
            const auto* g = value.getAsGroup();
            if (g == nullptr) { throw SerializationError("JSON value was not object type"); }
            detail::deserializeVisitor(*g, result);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to deserialize type ") + refl::getNameSafe<T>());
        }
    }

    static void deserializeFrom(const Value& val, const std::string& name, T& result) {
        try {
            priv::Serializer<T>::deserializeFrom(val, name, result, &deserialize);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to deserialize type ") + refl::getNameSafe<T>());
        }
    }

    static Value serialize(const T& value) { return detail::serializeVisitor(value); }

    static void serializeInto(Group& result, const std::string& name, const T& val) {
        try {
            priv::Serializer<T>::serializeInto(result, name, val, &serialize);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to serialize type ") + refl::getNameSafe<T>());
        }
    }

    static void deserializeStream(stream::InputStream& stream, T& result) {
        try {
            detail::deserializeStreamVisitor(stream, result);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to deserialize type ") + refl::getNameSafe<T>());
        }
    }

    static void serializeStream(stream::OutputStream& stream, const T& value, unsigned int tabSize,
                                unsigned int currentIndent) {
        try {
            detail::serializeStreamVisitor(stream, value, tabSize, currentIndent);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to serialize type ") + refl::getNameSafe<T>());
        }
    }
};

template<>
struct Serializer<bool, false> {
    static void deserialize(bool& result, const Value& v) {
        const bool* r = v.getAsBool();
        if (r != nullptr) { result = *r; }
        else { throw SerializationError("JSON value was not boolean type"); }
    }

    static void deserializeFrom(const Value& val, const std::string& name, bool& result) {
        priv::Serializer<bool>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(bool value) { return Value(value); }

    static void serializeInto(Group& result, const std::string& name, bool value) {
        priv::Serializer<bool>::serializeInto(result, name, value, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, bool& result) {
        json::Loader loader(stream);
        if (!loader.loadBool(result)) {
            throw SerializationError("Failed to read boolean from stream");
        }
    }

    static void serializeStream(stream::OutputStream& stream, bool value, unsigned int,
                                unsigned int) {
        try {
            stream << (value ? "true" : "false");
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON boolean value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename T>
struct Serializer<T, true> {
    static void deserialize(T& result, const Value& v) {
        if constexpr (std::is_enum_v<T>) {
            const long t = v.getNumericAsInteger();
            result       = static_cast<T>(t);
        }
        else if constexpr (std::is_floating_point_v<T>) { result = v.getNumericAsFloat(); }
        else { result = v.getNumericAsInteger(); }
    }

    static void deserializeFrom(const Value& val, const std::string& name, T& result) {
        priv::Serializer<T>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(T value) {
        if constexpr (std::is_enum_v<T>) { return Value(static_cast<long>(value)); }
        else if constexpr (std::is_floating_point_v<T>) { return Value(static_cast<float>(value)); }
        else { return Value(value); }
    }

    static void serializeInto(Group& result, const std::string& name, T value) {
        priv::Serializer<T>::serializeInto(result, name, value, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, T& result) {
        json::Loader loader(stream);
        Value v(0);
        if (!loader.loadNumeric(v)) {
            throw SerializationError("Failed to read numeric from stream");
        }
        if constexpr (std::is_enum_v<T>) {
            const long t = v.getNumericAsInteger();
            result       = static_cast<T>(t);
        }
        else if constexpr (std::is_floating_point_v<T>) { result = v.getNumericAsFloat(); }
        else { result = v.getNumericAsInteger(); }
    }

    static void serializeStream(stream::OutputStream& stream, T value, unsigned int, unsigned int) {
        try {
            if constexpr (std::is_enum_v<T>) {
                using U = std::underlying_type_t<T>;
                if constexpr (std::is_same_v<U, std::uint8_t>) {
                    stream << static_cast<unsigned int>(value);
                }
                else if constexpr (std::is_same_v<U, std::int8_t>) {
                    stream << static_cast<int>(value);
                }
                else { stream << static_cast<U>(value); }
            }
            else if constexpr (std::is_floating_point_v<T>) { stream << std::fixed << value; }
            else if constexpr (std::is_same_v<T, std::uint8_t>) {
                stream << static_cast<unsigned int>(value);
            }
            else if constexpr (std::is_same_v<T, std::int8_t>) {
                stream << static_cast<int>(value);
            }
            else { stream << value; }
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON numeric value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename U>
struct Serializer<U*, false> {
    using T = U*;

    static void deserialize(T result, const Value& v) {
        if (!result) { throw SerializationError("Cannot deserialize into null pointer"); }
        Serializer<U>::deserialize(*result, v);
    }

    static void deserializeFrom(const Value& val, const std::string& name, T result) {
        if (!result) { throw SerializationError("Cannot deserialize into null pointer"); }
        Serializer<U>::deserializeFrom(val, name, *result);
    }

    static Value serialize(const T value) {
        if (!value) { throw SerializationError("Cannot serialize null pointer"); }
        return Serializer<U>::serialize(*value);
    }

    static void serializeInto(Group& result, const std::string& name, const T value) {
        if (!value) { throw SerializationError("Cannot serialize null pointer"); }
        Serializer<U>::serializeInto(result, name, *value);
    }

    static void deserializeStream(stream::InputStream& stream, T result) {
        if (!result) { throw SerializationError("Cannot deserialize into null pointer"); }
        Serializer<U>::deserializeStream(stream, *result);
    }

    static void serializeStream(stream::OutputStream& stream, const T value, unsigned int tabSize,
                                unsigned int currentIndent) {
        if (!value) { throw SerializationError("Cannot serialize null pointer"); }
        Serializer<U>::serializeStream(stream, *value, tabSize, currentIndent);
    }
};

template<>
struct Serializer<std::string, false> {
    static void deserialize(std::string& result, const Value& v) {
        const std::string* r = v.getAsString();
        if (r != nullptr) { result = *r; }
        else { throw SerializationError("JSON value was not string type"); }
    }

    static void deserializeFrom(const Value& val, const std::string& name, std::string& result) {
        priv::Serializer<std::string>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const std::string& value) { return Value(value); }

    static void serializeInto(Group& result, const std::string& name, const std::string& value) {
        priv::Serializer<std::string>::serializeInto(result, name, value, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, std::string& result) {
        json::Loader loader(stream);
        if (!loader.loadString(result)) {
            throw SerializationError("Failed to read string from stream");
        }
    }

    static void serializeStream(stream::OutputStream& stream, const std::string& value,
                                unsigned int, unsigned int) {
        try {
            stream << '"';
            for (const char c : value) {
                switch (c) {
                case '\n':
                    stream << "\\n";
                    break;
                case '\t':
                    stream << "\\t";
                    break;
                case '"':
                    stream << "\\\"";
                    break;
                default:
                    stream << c;
                    break;
                }
            }
            stream << '"';
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON string value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename U, std::size_t N>
struct Serializer<U[N], false> {
    static void deserialize(U* result, const Value& v) {
        if (result == nullptr) { throw SerializationError("Cannot deserialize into null array"); }
        const List* r = v.getAsList();
        if (r == nullptr) { throw SerializationError("JSON value was not array type"); }
        if (r->size() != N) {
            throw SerializationError("JSON array size did not match expected fixed array size");
        }
        for (std::size_t i = 0; i < N; ++i) { Serializer<U>::deserialize(result[i], r->at(i)); }
    }

    static void deserializeFrom(const Value& val, const std::string& name, U* result) {
        priv::Serializer<U[N]>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const U* val) {
        List result;
        result.reserve(N);
        for (std::size_t i = 0; i < N; ++i) {
            result.emplace_back(Serializer<U>::serialize(val[i]));
        }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name, const U* val) {
        priv::Serializer<U[N]>::serializeInto(result, name, val, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, U* result) {
        if (result == nullptr) { throw SerializationError("Cannot deserialize into null array"); }
        StreamUtil::skipWhitespace(stream);
        if (stream.get() != '[') { throw SerializationError("Expected '[' for JSON array"); }
        StreamUtil::skipWhitespace(stream);

        unsigned int i = 0;
        while (i < N) {
            Serializer<U>::deserializeStream(stream, result[i]);
            ++i;
            StreamUtil::skipWhitespace(stream);
            switch (stream.get()) {
            case ',':
                if (i >= N) {
                    throw SerializationError("Too many JSON array elements for fixed array");
                }
                break;
            case ']':
                if (i == N) return;
                throw SerializationError("Too few JSON array elements for fixed array");
            default:
                throw SerializationError("Invalid JSON array separator");
            }
        }
        throw SerializationError("Expected closing ']' for JSON array");
    }

    static void serializeStream(stream::OutputStream& stream, const U* value, unsigned int tabSize,
                                unsigned int currentIndent) {
        if (value == nullptr) { throw SerializationError("Cannot serialize null array"); }
        try {
            stream << "[";
            if (tabSize > 0) stream << "\n";

            const unsigned int sc = currentIndent + tabSize;
            for (unsigned int i = 0; i < N; ++i) {
                StreamUtil::writeRepeated(stream, ' ', sc);
                Serializer<U>::serializeStream(stream, value[i], tabSize, sc);
                if (i < N - 1) stream << ",";
                if (tabSize > 0) stream << '\n';
            }

            StreamUtil::writeRepeated(stream, ' ', currentIndent);
            stream << ']';
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON array value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename U, std::size_t N>
struct Serializer<std::array<U, N>, false> {
    using T = std::array<U, N>;

    static void deserialize(T& result, const Value& v) {
        const List* r = v.getAsList();
        if (r == nullptr) { throw SerializationError("JSON value was not array type"); }
        if (r->size() != N) {
            throw SerializationError("JSON array size did not match expected std::array size");
        }
        for (std::size_t i = 0; i < N; ++i) { Serializer<U>::deserialize(result[i], r->at(i)); }
    }

    static void deserializeFrom(const Value& val, const std::string& name, T& result) {
        priv::Serializer<U[N]>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const T& val) {
        List result;
        result.reserve(N);
        for (std::size_t i = 0; i < N; ++i) {
            result.emplace_back(Serializer<U>::serialize(val[i]));
        }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name, const T& val) {
        priv::Serializer<U[N]>::serializeInto(result, name, val, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, T& result) {
        StreamUtil::skipWhitespace(stream);
        if (stream.get() != '[') { throw SerializationError("Expected '[' for JSON array"); }
        StreamUtil::skipWhitespace(stream);

        unsigned int i = 0;
        while (i < N) {
            Serializer<U>::deserializeStream(stream, result[i]);
            ++i;
            StreamUtil::skipWhitespace(stream);
            switch (stream.get()) {
            case ',':
                if (i >= N) {
                    throw SerializationError("Too many JSON array elements for std::array");
                }
                break;
            case ']':
                if (i == N) return;
                throw SerializationError("Too few JSON array elements for std::array");
            default:
                throw SerializationError("Invalid JSON array separator");
            }
        }
        throw SerializationError("Expected closing ']' for JSON array");
    }

    static void serializeStream(stream::OutputStream& stream, const T& value, unsigned int tabSize,
                                unsigned int currentIndent) {
        try {
            stream << "[";
            if (tabSize > 0) stream << "\n";

            const unsigned int sc = currentIndent + tabSize;
            for (unsigned int i = 0; i < N; ++i) {
                StreamUtil::writeRepeated(stream, ' ', sc);
                Serializer<U>::serializeStream(stream, value[i], tabSize, sc);
                if (i < N - 1) stream << ",";
                if (tabSize > 0) stream << '\n';
            }

            StreamUtil::writeRepeated(stream, ' ', currentIndent);
            stream << ']';
        } catch (const stream::StreamError& e) {
            throw SerializationError(e, "Failed to write JSON array value to stream");
        }
    }
};

template<typename U>
struct Serializer<std::vector<U>, false> {
    static void deserialize(std::vector<U>& result, const Value& v) {
        const List* r = v.getAsList();
        if (r == nullptr) { throw SerializationError("JSON value was not array type"); }
        result.resize(r->size());
        for (unsigned int i = 0; i < r->size(); ++i) {
            Serializer<U>::deserialize(result[i], r->at(i));
        }
    }

    static void deserializeFrom(const Value& val, const std::string& name, std::vector<U>& result) {
        priv::Serializer<std::vector<U>>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const std::vector<U>& value) {
        List result;
        result.reserve(value.size());
        for (const U& v : value) { result.push_back(Serializer<U>::serialize(v)); }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name, const std::vector<U>& value) {
        priv::Serializer<std::vector<U>>::serializeInto(result, name, value, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, std::vector<U>& result) {
        StreamUtil::skipWhitespace(stream);
        if (stream.get() != '[') { throw SerializationError("Expected '[' for JSON array"); }
        StreamUtil::skipWhitespace(stream);

        while (stream.isValid()) {
            if (stream.peek() == ']') {
                stream.get();
                return;
            }
            result.emplace_back();
            Serializer<U>::deserializeStream(stream, result.back());
            StreamUtil::skipWhitespace(stream);
            switch (stream.get()) {
            case ',':
                break;
            case ']':
                return;
            default:
                throw SerializationError("Invalid JSON array separator");
            }
        }
        throw SerializationError("Unexpected end of stream while reading JSON array");
    }

    static void serializeStream(stream::OutputStream& stream, const std::vector<U>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        try {
            stream << "[";
            if (tabSize > 0) stream << "\n";

            const unsigned int sc = currentIndent + tabSize;
            for (unsigned int i = 0; i < value.size(); ++i) {
                StreamUtil::writeRepeated(stream, ' ', sc);
                Serializer<U>::serializeStream(stream, value[i], tabSize, sc);
                if (i < value.size() - 1) stream << ",";
                if (tabSize > 0) stream << '\n';
            }

            StreamUtil::writeRepeated(stream, ' ', currentIndent);
            stream << ']';
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON array value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename U>
struct Serializer<std::unordered_set<U>, false> {
    static void deserialize(std::unordered_set<U>& result, const Value& v) {
        const List* r = v.getAsList();
        if (r == nullptr) { throw SerializationError("JSON value was not array type"); }
        for (unsigned int i = 0; i < r->size(); ++i) {
            U val;
            Serializer<U>::deserialize(val, r->at(i));
            result.emplace(std::move(val));
        }
    }

    static void deserializeFrom(const Value& val, const std::string& name,
                                std::unordered_set<U>& result) {
        priv::Serializer<std::unordered_set<U>>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const std::unordered_set<U>& value) {
        List result;
        result.reserve(value.size());
        for (const U& v : value) { result.push_back(Serializer<U>::serialize(v)); }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name,
                              const std::unordered_set<U>& value) {
        priv::Serializer<std::unordered_set<U>>::serializeInto(result, name, value, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, std::unordered_set<U>& result) {
        StreamUtil::skipWhitespace(stream);
        if (stream.get() != '[') { throw SerializationError("Expected '[' for JSON array"); }
        StreamUtil::skipWhitespace(stream);

        while (stream.isValid()) {
            U temp;
            Serializer<U>::deserializeStream(stream, temp);
            result.emplace(std::move(temp));
            StreamUtil::skipWhitespace(stream);
            switch (stream.get()) {
            case ',':
                break;
            case ']':
                return;
            default:
                throw SerializationError("Invalid JSON array separator");
            }
        }
        throw SerializationError("Unexpected end of stream while reading JSON array");
    }

    static void serializeStream(stream::OutputStream& stream, const std::unordered_set<U>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        try {
            stream << "[";
            if (tabSize > 0) stream << "\n";

            const unsigned int sc = currentIndent + tabSize;
            unsigned int i        = 0;
            for (const U& val : value) {
                StreamUtil::writeRepeated(stream, ' ', sc);
                Serializer<U>::serializeStream(stream, val, tabSize, sc);
                if (i < value.size() - 1) stream << ",";
                if (tabSize > 0) stream << '\n';
                ++i;
            }

            StreamUtil::writeRepeated(stream, ' ', sc);
            stream << ']';
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON array value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename U>
struct Serializer<std::unordered_map<std::string, U>, false> {
    static void deserialize(std::unordered_map<std::string, U>& result, const Value& v) {
        const Group* group = v.getAsGroup();
        if (group == nullptr) { throw SerializationError("JSON value was not object type"); }
        const Group& g = *group;
        for (const std::string& field : g.getFields()) {
            U f;
            Serializer<U>::deserialize(f, *g.getField(field));
            result.emplace(field, std::move(f));
        }
    }

    static void deserializeFrom(const Value& val, const std::string& name,
                                std::unordered_map<std::string, U>& result) {
        priv::Serializer<std::unordered_map<std::string, U>>::deserializeFrom(
            val, name, result, &deserialize);
    }

    static Value serialize(const std::unordered_map<std::string, U>& value) {
        Group result;
        for (const auto& p : value) {
            result.addField(p.first, Serializer<U>::serialize(p.second));
        }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name,
                              const std::unordered_map<std::string, U>& value) {
        priv::Serializer<std::unordered_map<std::string, U>>::serializeInto(
            result, name, value, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream,
                                  std::unordered_map<std::string, U>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) {
            throw SerializationError("Failed to read JSON map value from stream");
        }
        deserialize(result, val);
    }

    static void serializeStream(stream::OutputStream& stream,
                                const std::unordered_map<std::string, U>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        try {
            stream << "{";
            if (tabSize > 0) stream << "\n";

            const unsigned int sc = tabSize + currentIndent;
            unsigned int i        = 0;
            for (const auto& pair : value) {
                StreamUtil::writeRepeated(stream, ' ', sc);
                stream << '"' << pair.first << "\": ";
                Serializer<U>::serializeStream(stream, pair.second, tabSize, sc);
                if (i < value.size() - 1) stream << ',';
                if (tabSize > 0) stream << "\n";
                ++i;
            }
            StreamUtil::writeRepeated(stream, ' ', currentIndent);
            stream << "}";
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON map value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename U, typename V>
struct Serializer<std::unordered_map<U, V>, false> {
    static constexpr bool IsIntegral = std::is_integral_v<U> || std::is_enum_v<U>;
    static constexpr bool IsString   = std::is_same_v<U, std::string>;
    static constexpr bool IsStringCompat =
        std::is_constructible_v<U, std::string> && std::is_convertible_v<std::string, U>;
    using T = util::UnderlyingTypeT<U>;
    static_assert(IsIntegral || IsString || IsStringCompat, "Map must have integer or string keys");

    static U keyFromString(const std::string& s) {
        if constexpr (IsString) { return s; }
        else if constexpr (IsStringCompat) { return U(s); }
        else { return static_cast<U>(static_cast<T>(std::stoll(s))); }
    }

    static std::string keyToString(const U& k) {
        if constexpr (IsString) { return k; }
        else if constexpr (IsStringCompat) { return std::string(k); }
        else { return std::to_string(static_cast<T>(k)); }
    }

    static void deserialize(std::unordered_map<U, V>& result, const Value& v) {
        const Group* group = v.getAsGroup();
        if (group == nullptr) { throw SerializationError("JSON value was not object type"); }
        const Group& g = *group;
        for (const std::string& field : g.getFields()) {
            const U k = keyFromString(field);
            auto it   = result.try_emplace(k).first;
            Serializer<V>::deserialize(it->second, *g.getField(field));
        }
    }

    static void deserializeFrom(const Value& val, const std::string& name,
                                std::unordered_map<U, V>& result) {
        priv::Serializer<std::unordered_map<U, V>>::deserializeFrom(
            val, name, result, &deserialize);
    }

    static Value serialize(const std::unordered_map<U, V>& value) {
        Group result;
        for (const auto& p : value) {
            result.addField(keyToString(p.first), Serializer<V>::serialize(p.second));
        }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name,
                              const std::unordered_map<U, V>& value) {
        priv::Serializer<std::unordered_map<U, V>>::serializeInto(result, name, value, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, std::unordered_map<U, V>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) {
            throw SerializationError("Failed to read JSON map value from stream");
        }
        deserialize(result, val);
    }

    static void serializeStream(stream::OutputStream& stream, const std::unordered_map<U, V>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        try {
            stream << "{";
            if (tabSize > 0) stream << "\n";

            const unsigned int sc = tabSize + currentIndent;
            unsigned int i        = 0;
            for (const auto& pair : value) {
                StreamUtil::writeRepeated(stream, ' ', sc);
                stream << '"' << keyToString(pair.first) << "\": ";
                Serializer<V>::serializeStream(stream, pair.second, tabSize, sc);
                if (i < value.size() - 1) stream << ',';
                if (tabSize > 0) stream << "\n";
                ++i;
            }
            StreamUtil::writeRepeated(stream, ' ', currentIndent);
            stream << "}";
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON map value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<glm::length_t C, glm::length_t R, typename T, enum glm::qualifier Q>
struct Serializer<glm::mat<C, R, T, Q>, false> {
    using M = glm::mat<C, R, T, Q>;
    using S = Serializer<T>;

    static Value serialize(const M& v) {
        List cols;
        cols.reserve(C);
        for (unsigned int i = 0; i < C; ++i) {
            List col;
            col.reserve(R);
            for (unsigned int j = 0; j < R; ++j) { col.emplace_back(S::serialize(v[i][j])); }
            cols.emplace_back(col);
        }
        return {cols};
    }

    static void serializeInto(Group& g, const std::string& key, const M& val) {
        priv::Serializer<M>::serializeInto(g, key, val, &serialize);
    }

    static void deserialize(M& result, const Value& val) {
        const List* cols = val.getAsList();
        if (!cols || cols->size() != C) {
            throw SerializationError("JSON matrix column count did not match expected size");
        }
        for (unsigned int i = 0; i < C; ++i) {
            const List* col = cols->at(i).getAsList();
            if (!col || col->size() != R) {
                throw SerializationError("JSON matrix row count did not match expected size");
            }
            for (unsigned int j = 0; j < R; ++j) { S::deserialize(result[i][j], col->at(j)); }
        }
    }

    static void deserializeFrom(const Value& val, const std::string& key, M& result) {
        priv::Serializer<M>::deserializeFrom(val, key, result, &deserialize);
    }

    static void deserializeStream(stream::InputStream& stream, M& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) {
            throw SerializationError("Failed to read JSON matrix value from stream");
        }
        deserialize(result, val);
    }

    static void serializeStream(stream::OutputStream& stream, const M& value, unsigned int,
                                unsigned int) {
        try {
            stream << "[ ";
            for (unsigned int i = 0; i < C; ++i) {
                if (i > 0) stream << ", ";
                stream << "[ ";
                for (unsigned int j = 0; j < R; ++j) {
                    if (j > 0) stream << ", ";
                    S::serializeStream(stream, value[i][j], 0, 0);
                }
                stream << " ]";
            }
            stream << " ]";
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write JSON matrix value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<>
struct Serializer<script::Value, false> {
    static Value serialize(const script::Value& value) {
        const auto& val = value.value();

        Group g;
        g.addField("type", Serializer<script::PrimitiveValue::Type>::serialize(val.getType()));

        Value v(false);
        switch (val.getType()) {
        case script::PrimitiveValue::TBool:
            v = val.getAsBool();
            break;
        case script::PrimitiveValue::TInteger:
            v = val.getAsInt();
            break;
        case script::PrimitiveValue::TFloat:
            v = val.getAsFloat();
            break;
        case script::PrimitiveValue::TString:
            v = val.getAsString();
            break;
        case script::PrimitiveValue::TArray: {
            v               = List();
            const auto& arr = val.getAsArray();
            v.getAsList()->reserve(arr.size());
            for (const auto& iv : arr) { v.getAsList()->emplace_back(serialize(iv)); }
        } break;

        case script::PrimitiveValue::TFunction:
        case script::PrimitiveValue::TVoid:
        default:
            v = "<NONSERIALIZABLE>";
            break;
        }
        g.addField("value", v);

        Group props;
        for (const auto& prop : value.allProperties()) {
            props.addField(prop.first, serialize(prop.second.deref()));
        }
        g.addField("props", props);

        return {g};
    }

    static void serializeInto(Group& group, const std::string& name, const script::Value& val) {
        priv::Serializer<script::Value>::serializeInto(group, name, val, &serialize);
    }

    static void deserialize(script::Value& result, const Value& val) {
        const Group* group = val.getAsGroup();
        if (!group) { throw SerializationError("JSON value was not object type"); }

        script::PrimitiveValue::Type type;
        Serializer<script::PrimitiveValue::Type>::deserializeFrom(val, "type", type);

        const Value* v = group->getField("value");
        if (!v) { throw SerializationError("JSON field not found: value"); }

        switch (type) {
        case script::PrimitiveValue::TBool:
            if (!v->getAsBool()) { throw SerializationError("JSON value was not boolean type"); }
            result.value() = *v->getAsBool();
            break;

        case script::PrimitiveValue::TInteger:
            if (!v->getAsInteger() && !v->getAsFloat()) {
                throw SerializationError("JSON value was not numeric type");
            }
            result.value() = v->getNumericAsInteger();
            break;

        case script::PrimitiveValue::TFloat:
            if (!v->getAsInteger() && !v->getAsFloat()) {
                throw SerializationError("JSON value was not numeric type");
            }
            result.value() = v->getNumericAsFloat();
            break;

        case script::PrimitiveValue::TString:
            if (!v->getAsString()) { throw SerializationError("JSON value was not string type"); }
            result.value() = *v->getAsString();
            break;

        case script::PrimitiveValue::TArray: {
            const List* list = v->getAsList();
            if (!list) { throw SerializationError("JSON value was not array type"); }

            std::vector<script::Value> arr;
            arr.resize(list->size());
            for (unsigned int i = 0; i < list->size(); ++i) { deserialize(arr[i], list->at(i)); }
            result = arr;
        } break;

        case script::PrimitiveValue::TRef:
        case script::PrimitiveValue::TFunction:
        case script::PrimitiveValue::TVoid:
        default:
            result.value() = false;
            break;
        }

        const Group* props = group->getGroup("props");
        if (props) {
            for (const std::string& prop : props->getFields()) {
                script::Value pval;
                deserialize(pval, *props->getField(prop));
                result.setProperty(prop, {std::move(pval)});
            }
        }
    }

    static void deserializeFrom(const Value& val, const std::string& key, script::Value& result) {
        priv::Serializer<script::Value>::deserializeFrom(val, key, result, &deserialize);
    }

    static void deserializeStream(stream::InputStream& stream, script::Value& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) {
            throw SerializationError("Failed to read script value from stream");
        }
        deserialize(result, val);
    }

    static void serializeStream(stream::OutputStream& stream, const script::Value& value,
                                unsigned int, unsigned int) {
        const Value val = serialize(value);
        try {
            stream << val;
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write script value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename U>
struct Serializer<ctr::Vector2D<U>, false> {
    static Value serialize(const ctr::Vector2D<U>& v) {
        Group g;
        g.addField("w", Serializer<unsigned int>::serialize(v.getWidth()));
        g.addField("h", Serializer<unsigned int>::serialize(v.getHeight()));
        List data;
        data.reserve(v.getHeight() * v.getWidth());
        for (unsigned int x = 0; x < v.getWidth(); ++x) {
            for (unsigned int y = 0; y < v.getHeight(); ++y) {
                data.emplace_back(Serializer<U>::serialize(v(x, y)));
            }
        }
        g.addField("data", {data});
        return {g};
    }

    static void serializeInto(Group& g, const std::string& key, const ctr::Vector2D<U>& val) {
        priv::Serializer<ctr::Vector2D<U>>::serializeInto(g, key, val, &serialize);
    }

    static void deserialize(ctr::Vector2D<U>& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) { throw SerializationError("JSON value was not object type"); }
        const Group& g    = *rg;
        const Value* w    = g.getField("w");
        const Value* h    = g.getField("h");
        const Value* data = g.getField("data");
        if (!w || !h || !data) {
            throw SerializationError("JSON Vector2D value missing required fields");
        }
        if (w->getType() != Value::Type::Integer || h->getType() != Value::Type::Integer ||
            data->getType() != Value::Type::List) {
            throw SerializationError("JSON Vector2D fields had invalid types");
        }
        const unsigned int mx = *w->getAsInteger();
        const unsigned int my = *h->getAsInteger();
        const List& d         = *data->getAsList();
        if (d.size() != mx * my) {
            throw SerializationError("JSON Vector2D data size did not match dimensions");
        }
        result.setSize(mx, my);
        for (unsigned int x = 0; x < mx; ++x) {
            for (unsigned int y = 0; y < my; ++y) {
                Serializer<U>::deserialize(result(x, y), d[x * my + y]);
            }
        }
    }

    static void deserializeFrom(const Value& val, const std::string& key,
                                ctr::Vector2D<U>& result) {
        priv::Serializer<ctr::Vector2D<U>>::deserializeFrom(val, key, result, &deserialize);
    }

    static void deserializeStream(stream::InputStream& stream, ctr::Vector2D<U>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) {
            throw SerializationError("Failed to read Vector2D value from stream");
        }
        deserialize(result, val);
    }

    static void serializeStream(stream::OutputStream& stream, const ctr::Vector2D<U>& value,
                                unsigned int, unsigned int) {
        const Value val = serialize(value);
        try {
            stream << val;
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write Vector2D value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<typename... Ts>
struct Serializer<std::variant<Ts...>, false> {
private:
    struct HelperBase {
        virtual void deserialize(const Value& input, std::variant<Ts...>& v) const = 0;
    };

    template<typename U>
    struct Helper : public HelperBase {
        virtual void deserialize(const Value& input, std::variant<Ts...>& v) const override {
            v.template emplace<U>();
            Serializer<U>::deserialize(std::get<U>(v), input);
        }
    };

public:
    static void deserialize(std::variant<Ts...>& result, const Value& v) {
        const Group* group = v.getAsGroup();
        if (!group) { throw SerializationError("JSON value was not object type"); }
        const Value* i = group->getField("index");
        const Value* d = group->getField("data");
        if (!i || !d) { throw SerializationError("JSON variant missing required fields"); }
        if (i->getType() != Value::Type::Integer) {
            throw SerializationError("JSON variant index field was not integer type");
        }
        const unsigned int index = *i->getAsInteger();
        if (index >= sizeof...(Ts)) { throw SerializationError("JSON variant index out of range"); }

        static const std::tuple<Helper<Ts>...> helperTuple(Helper<Ts>{}...);
        static const HelperBase* helpers[] = {&std::get<Helper<Ts>>(helperTuple)...};
        helpers[index]->deserialize(*d, result);
    }

    static void deserializeFrom(const Value& val, const std::string& name,
                                std::variant<Ts...>& result) {
        priv::Serializer<std::variant<Ts...>>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const std::variant<Ts...>& value) {
        Group result;
        result.addField("index", value.index());

        static const auto visitor = [](const auto& c) -> Value {
            return Serializer<std::decay_t<decltype(c)>>::serialize(c);
        };
        result.addField("data", std::visit(visitor, value));

        return {result};
    }

    static void serializeInto(Group& result, const std::string& name,
                              const std::variant<Ts...>& value) {
        priv::Serializer<std::variant<Ts...>>::serializeInto(result, name, value, &serialize);
    }

    static void deserializeStream(stream::InputStream& stream, std::variant<Ts...>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) {
            throw SerializationError("Failed to read variant value from stream");
        }
        deserialize(result, val);
    }

    static void serializeStream(stream::OutputStream& stream, const std::variant<Ts...>& value,
                                unsigned int, unsigned int) {
        const Value val = serialize(value);
        try {
            stream << val;
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write variant value to stream";
            throw SerializationError(e, message);
        }
    }
};

template<>
struct Serializer<sf::Image, false> {
    static Value serialize(const sf::Image& image) {
        const auto data = image.saveToMemory("png");
        if (!data) { throw SerializationError("Failed to encode image as PNG"); }

        return Value(util::Base64::encode(
            std::span<const char>(reinterpret_cast<const char*>(data->data()), data->size())));
    }

    static void serializeInto(Group& g, const std::string& key, const sf::Image& val) {
        priv::Serializer<sf::Image>::serializeInto(g, key, val, &serialize);
    }

    static void deserialize(sf::Image& result, const Value& val) {
        const std::string* data = val.getAsString();
        if (!data) { throw SerializationError("JSON value was not string type"); }
        std::vector<char> decoded;
        if (!util::Base64::decode(*data, decoded)) {
            throw SerializationError("Failed to decode base64 image data");
        }
        if (!result.loadFromMemory(decoded.data(), decoded.size())) {
            throw SerializationError("Failed to decode image bytes");
        }
    }

    static void deserializeFrom(const Value& val, const std::string& key, sf::Image& result) {
        priv::Serializer<sf::Image>::deserializeFrom(val, key, result, &deserialize);
    }

    static void serializeStream(stream::OutputStream& stream, const sf::Image& value, unsigned int,
                                unsigned int) {
        const Value encoded = serialize(value);
        try {
            stream << encoded;
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write image value to stream";
            throw SerializationError(e, message);
        }
    }

    static void deserializeStream(stream::InputStream& stream, sf::Image& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) {
            throw SerializationError("Failed to read image value from stream");
        }
        deserialize(result, val);
    }
};

template<typename T>
struct Serializer<std::optional<T>, false> {
    static Value serialize(const std::optional<T>& opt) {
        Group g;
        if (opt) {
            g.addField("has_value", true);
            g.addField("value", Serializer<T>::serialize(*opt));
        }
        else { g.addField("has_value", false); }
        return {g};
    }

    static void serializeInto(Group& g, const std::string& key, const std::optional<T>& val) {
        priv::Serializer<std::optional<T>>::serializeInto(g, key, val, &serialize);
    }

    static void deserialize(std::optional<T>& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) { throw SerializationError("JSON value was not object type"); }
        const Group& g  = *rg;
        const Value* hv = g.getField("has_value");
        if (!hv || hv->getType() != Value::Type::Bool) {
            throw SerializationError("JSON optional missing valid has_value field");
        }
        if (!*hv->getAsBool()) {
            result.reset();
            return;
        }
        const Value* v = g.getField("value");
        if (!v) { throw SerializationError("JSON optional missing value field"); }
        result.emplace();
        Serializer<T>::deserialize(*result, *v);
    }

    static void deserializeFrom(const Value& val, const std::string& key,
                                std::optional<T>& result) {
        priv::Serializer<std::optional<T>>::deserializeFrom(val, key, result, &deserialize);
    }

    static void deserializeStream(stream::InputStream& stream, std::optional<T>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) {
            throw SerializationError("Failed to read optional value from stream");
        }
        deserialize(result, val);
    }

    static void serializeStream(stream::OutputStream& stream, const std::optional<T>& value,
                                unsigned int, unsigned int) {
        try {
            stream << "{ ";
            stream << "\"has_value\": " << (value.has_value() ? "true" : "false");
            if (value.has_value()) {
                stream << ", \"value\": ";
                Serializer<T>::serializeStream(stream, *value, 0, 0);
            }
            stream << " }";
        } catch (const stream::StreamError& e) {
            const std::string message = "Failed to write optional value to stream";
            throw SerializationError(e, message);
        }
    }
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
