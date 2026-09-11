#ifndef BLIB_RANDOM_UUID_HPP
#define BLIB_RANDOM_UUID_HPP

#include <BLIB/Serialization/Binary/Serializer.hpp>
#include <BLIB/Serialization/JSON/Serializer.hpp>
#include <BLIB/Util/HashCombine.hpp>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

namespace bl
{
namespace rand
{
/**
 * @brief UUID Generator
 *
 * @ingroup Random
 */
class UUID {
public:
    /// The fixed length of UUID strings
    static constexpr std::size_t StringLength = 36;

    /**
     * @brief Generates a new UUID string
     *
     * @return A new UUID string
     */
    static UUID generate();

    /**
     * @brief Creates a 0 UUID
     */
    UUID();

    /**
     * @brief Copies the UUID
     *
     * @param copy The UUID to copy
     */
    UUID(const UUID& copy) = default;

    /**
     * @brief Constructs the UUID from a string
     *
     * @param str The string to parse and construct from
     */
    UUID(std::string_view str);

    /**
     * @brief Constructs the UUID from a string
     *
     * @param str The string to parse and construct from
     */
    UUID(const std::string& str);

    /**
     * @brief Constructs the UUID from a string
     *
     * @param str The string to parse and construct from
     */
    UUID(const char* str);

    /**
     * @brief Constructs the UUID from its two 64 bit parts
     *
     * @param part1 The first 64 bits of the UUID
     * @param part2 The second 64 bits of the UUID
     */
    UUID(std::uint64_t part1, std::uint64_t part2);

    /**
     * @brief Copies the UUID
     *
     * @param copy The UUID to copy
     * @return A reference to this object
     */
    UUID& operator=(const UUID& copy) = default;

    /**
     * @brief Compare two UUIDs for equality
     *
     * @param other The UUID to compare to
     * @return True if the UUIDs are equal, false otherwise
     */
    bool operator==(const UUID& other) const {
        return part1 == other.part1 && part2 == other.part2;
    }

    /**
     * @brief Parses the UUID from a string. Only modifies if parsing is successful
     *
     * @param str The string to parse
     * @return True if the string was successfully parsed, false otherwise
     */
    bool parse(std::string_view str);

    /**
     * @brief Converts the UUID to a string
     */
    std::string toString() const;

    /**
     * @brief Converts the UUID to a string
     */
    operator std::string() const { return toString(); }

    /**
     * @brief Returns the first part of the UUID
     */
    std::uint64_t getPart1() const { return part1; }

    /**
     * @brief Returns the second part of the UUID
     */
    std::uint64_t getPart2() const { return part2; }

private:
    std::uint64_t part1;
    std::uint64_t part2;
};

inline std::ostream& operator<<(std::ostream& os, const UUID& uuid) {
    os << uuid.toString();
    return os;
}

} // namespace rand

namespace serial
{
namespace binary
{
template<>
struct Serializer<rand::UUID> {
    static bool serialize(stream::OutputStream& output, const rand::UUID& uuid) {
        detail::OutputStreamWrapper wrapper(output);
        if (!wrapper.write<std::uint64_t>(uuid.getPart1())) return false;
        return wrapper.write<std::uint64_t>(uuid.getPart2());
    }
    static bool deserialize(stream::InputStream& input, rand::UUID& uuid) {
        detail::InputStreamWrapper wrapper(input);
        std::uint64_t part1, part2;
        if (!wrapper.read<std::uint64_t>(part1)) return false;
        if (!wrapper.read<std::uint64_t>(part2)) return false;
        uuid = rand::UUID(part1, part2);
        return true;
    }
    static std::uint32_t size(const rand::UUID&) { return sizeof(std::uint64_t) * 2; }
};
} // namespace binary

namespace json
{
template<>
struct Serializer<rand::UUID> {
    static bool deserialize(rand::UUID& result, const Value& v) {
        const std::string* r = v.getAsString();
        if (r != nullptr) {
            result = rand::UUID(*r);
            return true;
        }
        return false;
    }

    static bool deserializeFrom(const Value& val, const std::string& name, rand::UUID& result) {
        return priv::Serializer<rand::UUID>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const rand::UUID& value) { return Value(value.toString()); }

    static void serializeInto(Group& result, const std::string& name, const rand::UUID& value) {
        priv::Serializer<rand::UUID>::serializeInto(result, name, value, &serialize);
    }

    static bool deserializeStream(stream::InputStream& stream, rand::UUID& result) {
        json::Loader loader(stream);
        std::string str;
        if (!loader.loadString(str)) { return false; }
        result = rand::UUID(str);
        return true;
    }

    static bool serializeStream(stream::OutputStream& stream, const rand::UUID& value, unsigned int,
                                unsigned int) {
        return Serializer<std::string>::serializeStream(stream, value.toString(), 0, 0);
    }
};
} // namespace json
} // namespace serial
} // namespace bl

namespace std
{
template<>
struct hash<bl::rand::UUID> {
    std::size_t operator()(const bl::rand::UUID& uuid) const noexcept {
        std::size_t h1 = std::hash<std::uint64_t>{}(uuid.getPart1());
        std::size_t h2 = std::hash<std::uint64_t>{}(uuid.getPart2());
        return bl::util::hashCombine(h1, h2);
    }
};
} // namespace std

#endif
